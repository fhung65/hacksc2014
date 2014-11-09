package myotank;

import java.io.OutputStream;
import java.io.IOException;

public class TankOutRunnable implements Runnable
{
	private DataManager m_dataManager;
	private OutputStream m_out;

	public TankOutRunnable(DataManager dataManager, OutputStream out)
	{
		m_dataManager = dataManager;
		m_out = out;
	}
	
	public void run()
	{
		while (!m_dataManager.tankThreadsKilled())
		{
			try {
				byte[] myoData = m_dataManager.getMyoData();
				if (myoData != null)
				{
					m_out.write(myoData);
					m_out.flush();
				}
			}
			catch (IOException e) {
				m_dataManager.setTankThreadsKilled(true);
			}
			try {
				Thread.sleep(200);
			}
			catch (InterruptedException e)
			{ }
		}
	}
}