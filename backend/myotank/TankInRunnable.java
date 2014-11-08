package myotank;

import java.io.InputStream;
import java.io.IOException;

public class TankInRunnable implements Runnable
{
	private DataManager m_dataManager;
	private InputStream m_in;
	
	public TankInRunnable(DataManager dataManager, InputStream in)
	{
		m_dataManager = dataManager;
		m_in = in;
	}
	
	public void run()
	{
		byte[] lengthData = new byte[4];
		int length;
		while (!m_dataManager.tankThreadsKilled())
		{
			try {
				int n = 0;
				while (n < 4)
					n += m_in.read(lengthData, n, 4 - n);
				length = lengthData[0];
				length |= lengthData[1] << 8;
				length |= lengthData[2] << 16;
				length |= lengthData[3] << 24;
				System.out.println("" + length);
				byte[] data = new byte[length];
				n = 0;
				while (n < length)
					n += m_in.read(data, n, length - n);
				m_dataManager.setImageData(data);
			}
			catch (IOException e) {
				m_dataManager.setTankThreadsKilled(true);
			}
		}
	}
}