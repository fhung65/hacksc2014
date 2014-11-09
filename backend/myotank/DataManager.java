package myotank;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;

public class DataManager
{
	private boolean m_tankThreadsKilled = false;
	private boolean m_clientThreadsKilled = false;
	private byte[] m_imageData = null;
	private boolean m_hasNewImageData = false;
	private LinkedList<byte[]> m_myoData = new LinkedList<byte[]>();
	private int screenshotIndex;
	private Object tankThreadLock = new Object();
	private Object clientThreadLock = new Object();
	private Object imageDataLock = new Object();
	private Object myoDataLock = new Object();
	
	public DataManager()
	{
		// Find appropriate number to begin screenshotting with
		StringBuffer stringBuffer = new StringBuffer("0000");
		for (screenshotIndex = 1; screenshotIndex < 10000; screenshotIndex++)
		{
			for (int i = 3; i >= 0; i--)
			{
				char c = stringBuffer.charAt(i);
				if (c != '9')
				{
					stringBuffer.setCharAt(i, (char)(c + 1));
					break;
				}
				else
					stringBuffer.setCharAt(i, '0');
			}
			File file = new File("C:\\RobotBackend\\Screenshots\\Screenshot" + stringBuffer.toString() + ".jpg");
			if (!file.isFile())
				break;
		}
		// Not worth our time to deal with the case where the user has over 10000 photos
	}
	
	public void setTankThreadsKilled(boolean value)
	{
		synchronized (tankThreadLock) {
			m_tankThreadsKilled = value;
		}
	}

	public boolean tankThreadsKilled()
	{
		synchronized (tankThreadLock) {
			return m_tankThreadsKilled;
		}
	}
	
	public void setClientThreadsKilled(boolean value)
	{
		synchronized (clientThreadLock) {
			m_clientThreadsKilled = value;
		}
	}
	
	public boolean clientThreadsKilled()
	{
		synchronized (clientThreadLock) {
			return m_clientThreadsKilled;
		}
	}
	
	public void setImageData(byte[] data)
	{
		synchronized (imageDataLock) {
			m_imageData = data;
			m_hasNewImageData = true;
		}
	}
	
	public byte[] getImageData()
	{
		synchronized (imageDataLock) {
			m_hasNewImageData = false;
			return m_imageData;
		}
	}
	
	public boolean hasNewImageData()
	{
		synchronized (imageDataLock) {
			return m_hasNewImageData;
		}
	}
	
	public void parseAndSetMyoData(byte[] input)
	{
		// Multiple inputs may be placed in the same line separated by semi-colons
		String[] commands = (new String(input)).trim().split(";");
		// Commands take form of type:magnitude
		ArrayList<String[]> validCommands = new ArrayList<String[]>();
		for (int i = 0; i < commands.length; i++)
			if (!commands[i].equals(""))
				validCommands.add(commands[i].split(":"));
		
		for (int i = 0; i < validCommands.size(); i++)
		{
			if (validCommands.get(i).length == 1) // Only screenshot command has no magnitude
			{
				// Save screenshot to server
				byte[] imageData = getImageData();
				if (imageData != null)
				{
					StringBuffer stringBuffer = new StringBuffer("0000");
					int multiplier = 1000;
					for (int i = 0; i < 4; i++)
					{
						stringBuffer.setCharAt(i, (char)((screenshotIndex / multiplier) % 10));
						multiplier /= 10;
					}
					FileOutputStream screenshot = null;
					try {
						screenshot = new FileOutputStream("C:\\RobotBackend\\Screenshots\\Screenshot" + stringBuffer.toString() + ".jpg");
					}
					catch (FileNotFoundException e)
					{ }
					screenshotIndex++;
					try {
						screenshot.write(imageData);
						screenshot.close();
					}
					catch (IOException e)
					{ }
				}	
			}
			else
			{
				double magnitude;
				try {
					magnitude = Double.parseDouble(validCommands.get(i)[1]);
				}
				catch (NumberFormatException e) {
					magnitude = 0.0;
				}
				synchronized (myoDataLock) {
					m_myoData.add(input);
				}
			}
		}
	}
	
	public byte[] getMyoData()
	{
		synchronized (myoDataLock) {
			/// TODO: Make this comply with communication standards
			return m_myoData.getFirst();
		}
	}
	
	private char parseCommand(String command)
	{
		if (command.equals("tr"))
			return 'r';
		if (command.equals("tl"))
			return 'l';
		if (command.equals("tf"))
			return 'f';
		if (command.equals("cr"))
			return 'a';
		return 'f';
	}
}