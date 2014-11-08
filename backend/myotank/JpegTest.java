package myotank;

import java.io.RandomAccessFile;
import java.io.OutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.InetAddress;

public class JpegTest
{
	public static void main(String[] args)
	{
		try
		{
			byte[] address = {10, 120, 88, 80};
			Socket server = new Socket(InetAddress.getByAddress(address), 1420);
			OutputStream out = server.getOutputStream();
			RandomAccessFile in = new RandomAccessFile("myotank\\corgi.jpg", "r");
			int length = (int)in.length();
			byte[] lengthData = new byte[4];
			lengthData[0] = (byte)(length & 0xFF);
			lengthData[1] = (byte)((length >> 8) & 0xFF);
			lengthData[2] = (byte)((length >> 16) & 0xFF);
			lengthData[3] = (byte)((length >> 24) & 0xFF);
			byte[] imageData = new byte[length];
			in.read(imageData);

			out.write(lengthData);
			out.write(imageData);
		}
		catch (IOException e)
		{
		}
	}
}