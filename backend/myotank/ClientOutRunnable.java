package myotank;

import java.io.IOException;
import java.io.OutputStream;

public class ClientOutRunnable implements Runnable
{
	private DataManager m_dataManager;
	private OutputStream m_out;

	public ClientOutRunnable(DataManager dataManager, OutputStream out)
	{
		m_dataManager = dataManager;
		m_out = out;
	}
	
	public void run()
	{
		while (!m_dataManager.clientThreadsKilled())
		{
			if (m_dataManager.hasNewImageData())
			{
				byte[] lengthData = new byte[4];
				byte[] imageData = m_dataManager.getImageData();
				lengthData[0] = (byte)((imageData.length << 1) & 0xFE);
				lengthData[1] = (byte)((imageData.length >> 6) & 0xFE);
				lengthData[2] = (byte)((imageData.length >> 13) & 0xFE);
				lengthData[3] = (byte)((imageData.length >> 20) & 0xFE);
				try {
					m_out.write(lengthData);
					m_out.flush();
					m_out.write(imageData);
					m_out.flush();
				}
				catch (IOException e) {
					m_dataManager.setClientThreadsKilled(true);
				}
			}
			else
			{
				try {
					Thread.sleep(100);
				}
				catch (InterruptedException e)
				{ }
			}
		}
	}
}