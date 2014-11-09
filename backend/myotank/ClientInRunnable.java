package myotank;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class ClientInRunnable implements Runnable
{
	private DataManager m_dataManager;
	private InputStream m_in;

	public ClientInRunnable(DataManager dataManager, InputStream in)
	{
		m_dataManager = dataManager;
		m_in = in;
	}
	
	public void run()
	{
		while (!m_dataManager.clientThreadsKilled())
		{
			byte[] data = new byte[1];
			try {
				ArrayList<Byte> string = new ArrayList<Byte>();
				boolean keepLooping = true;
				do
				{
					int bytesRead = m_in.read(data);
					if (bytesRead != -1)
					{
						if (data[0] == '\0')
							keepLooping = false;
						else
							string.add(data[0]);
					}
				} while (keepLooping);
				byte[] result = new byte[string.size()];
				for (int i = 0; i < string.size(); i++)
					result[i] = string.get(i);
				m_dataManager.parseAndSetMyoData(result);
			}
			catch (IOException e) {
				m_dataManager.setClientThreadsKilled(true);
			}
		}
	}
}