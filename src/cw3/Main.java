import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {
    private static int byteKey(int index, int seed) {
        return (seed + index * 17) & 0xFF;
    }

    private static void xorDecode(char[] buffer, int seed) {
        for (int index = 0; index < buffer.length; index++) {
            buffer[index] = (char) (buffer[index] ^ byteKey(index, seed));
        }
    }

    public static void main(String[] args) throws IOException {
        final int seed = 0x4D;
        char[] password = { 0x003E, 0x003B, 0x0004, 0x00F2, 0x00F4, 0x00D6 };
        char[] message = { 0x0018, 0x0035, 0x001D, 0x00F9, 0x00E5, 0x00DB, 0x0093, 0x00B0, 0x00B0, 0x008D, 0x0084, 0x007C };

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

        System.out.print("Podaj haslo: ");
        String input = reader.readLine();
        if (input == null) {
            return;
        }

        input = input.strip();
        xorDecode(password, seed);

        if (input.equals(new String(password))) {
            xorDecode(message, seed);
            System.out.println(new String(message));
        } else {
            System.out.println("Bledne haslo");
        }
    }
}
