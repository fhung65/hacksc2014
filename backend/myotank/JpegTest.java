package myotank;

import java.io.RandomAccessFile;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.InetAddress;

public class JpegTest
{
	private static final int MAX_CHUNK_SIZE = 4096;

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
			lengthData[0] = (byte)((length << 1) & 0xFE);
			lengthData[1] = (byte)((length >> 6) & 0xFE);
			lengthData[2] = (byte)((length >> 13) & 0xFE);
			lengthData[3] = (byte)((length >> 20) & 0xFE);
			System.out.println(lengthData[0] + " " + lengthData[1] + " " + lengthData[2] + " " + lengthData[3]);
			System.out.println(length + "");
			byte[] imageData = new byte[length];

			/*byte[] imageData = new byte[MAX_CHUNK_SIZE];
			out.write(lengthData);
			int n = 0;
			while (n < length)
			{
				int nBytesRead = in.read(imageData, 0, MAX_CHUNK_SIZE);
				out.write(imageData, 0, nBytesRead);
				n += nBytesRead;
			}*/
			in.read(imageData);
			out.write(lengthData);
			out.flush();
			out.write(imageData);
			out.flush();
			
			in = new RandomAccessFile("myotank\\wow.jpg", "r");
			length = (int)in.length();
			lengthData = new byte[4];
			lengthData[0] = (byte)((length << 1) & 0xFE);
			lengthData[1] = (byte)((length >> 6) & 0xFE);
			lengthData[2] = (byte)((length >> 13) & 0xFE);
			lengthData[3] = (byte)((length >> 20) & 0xFE);
			System.out.println(lengthData[0] + " " + lengthData[1] + " " + lengthData[2] + " " + lengthData[3]);
			System.out.println(length + "");
			imageData = new byte[length];
			
			in.read(imageData);
			out.write(lengthData);
			out.flush();
			out.write(imageData);
			out.flush();
			
			/*int n = 0;
			while (n < imageData.length)
			{
				out.write(imageData, n, Math.min(imageData.length - n, MAX_CHUNK_SIZE));
				n += MAX_CHUNK_SIZE;
				out.flush();
			}*/
			
			/*Socket server2 = new Socket(InetAddress.getByAddress(address), 2420);
			InputStream in2 = server2.getInputStream();
			RandomAccessFile savedFile = new RandomAccessFile("myotank\\result.jpg", "rw");
			byte[] lengthData2 = new byte[4];
			in2.read(lengthData2);
			int length2 = (lengthData2[0] & 0xFF);
			length2 |= ((lengthData2[1] & 0xFF) << 8);
			length2 |= ((lengthData2[2] & 0xFF) << 16);
			length2 |= ((lengthData2[3] & 0xFF) << 24);
			byte[] imageData2 = new byte[length2];
			in2.read(imageData2, 0, imageData2.length);
			savedFile.write(imageData2, 0, imageData2.length);
			/*byte[] imageData2 = new byte[MAX_CHUNK_SIZE];
			int n = 0;
			while (n < length2)
			{
				int nRead = in2.read(imageData2);
				savedFile.write(imageData2, 0, nRead);
				n += nRead;
			}*/
			
			in.close();
			out.close();
			//in2.close();
			//savedFile.close();
		}
		catch (IOException e)
		{
		}
	}
}