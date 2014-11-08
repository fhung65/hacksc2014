package myotank;

public class DataManager
{
	private boolean m_tankThreadsKilled = false;
	private boolean m_clientThreadsKilled = false;
	private Object tankThreadLock = new Object();
	private Object clientThreadLock = new Object();
	
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
}