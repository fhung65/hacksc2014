package myotank;

public class DataManager
{
	private boolean m_tankThreadsKilled = false;
	private boolean m_clientThreadsKilled = false;
	private byte[] m_imageData = null;
	private boolean m_hasNewImageData = false;
	private byte m_myoData = 0;
	private Object tankThreadLock = new Object();
	private Object clientThreadLock = new Object();
	private Object imageDataLock = new Object();
	private Object myoDataLock = new Object();
	
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
	
	public void setMyoData(byte data)
	{
		synchronized (myoDataLock) {
			m_myoData = data;
		}
	}
	
	public byte getMyoData()
	{
		synchronized (myoDataLock) {
			return m_myoData;
		}
	}
}