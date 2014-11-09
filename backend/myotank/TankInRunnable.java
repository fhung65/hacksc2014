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
				{
					int bytesRead = m_in.read(lengthData, n, 4 - n);
					if (bytesRead == -1)
						throw new IOException();
					n += bytesRead;
				}
				length = (lengthData[0] & 0xFE) >> 1;
				length |= (lengthData[1] & 0xFE) << 6;
				length |= (lengthData[2] & 0xFE) << 13;
				length |= (lengthData[3] & 0xFE) << 20;
				System.out.println(length + "");
				byte[] data = new byte[length];
				n = 0;
				while (n < length)
				{
					int bytesRead = m_in.read(data, n, length - n);
					if (bytesRead == -1)
						throw new IOException();
					n += bytesRead;
				}
				m_dataManager.setImageData(data);
			}
			catch (IOException e) {
				m_dataManager.setTankThreadsKilled(true);
			}
		}
	}
}