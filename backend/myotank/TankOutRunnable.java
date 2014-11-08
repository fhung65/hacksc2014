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
				m_out.write(m_dataManager.getMyoData());
			}
			catch (IOException e) {
				m_dataManager.setTankThreadsKilled(true);
			}
			try {
				Thread.sleep(1000);
			}
			catch (InterruptedException e)
			{ }
		}
	}
}