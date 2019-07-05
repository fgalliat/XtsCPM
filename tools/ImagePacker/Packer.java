import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;

import javax.imageio.ImageIO;

public class Packer {

    public static void main(String[] args) throws Exception {

        int start = 1;
        int nb = 8;
        nb = 4;

        int w = -1;
        int h = -1;

        // just 1 pack
        for(int c=0; c < 1; c++) {
            System.out.println("Packing char #"+c);

            int[][] rgbs = new int[nb][];
            for(int i=0; i < nb; i++) {
                String filename = "./imgs_own/img_"+( start + i )+".jpg";
                System.out.println(filename);
                BufferedImage img = ImageIO.read( new File( filename) );
                if ( w == -1 ) {
                    w = img.getWidth();
                    h = img.getHeight();
                }
                rgbs[i] = img.getRGB(0, 0, w, h, null, 0, w); 
            }
    
            produceFile("pack"+(1+c)+".pak", w, h, nb, rgbs);
            start += nb;
        }

        
    }

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static int color565(int r, int g, int b) {
        int intel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        return intel;
	}

	// //color565toRGB		- converts 565 format 16 bit color to RGB
	// static void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
	// 	r = (color>>8)&0x00F8;
	// 	g = (color>>3)&0x00FC;
	// 	b = (color<<3)&0x00F8;
	// }



    // beware -> will have to reverse rgbs endian & rgb565 those
    public static void produceFile(String filename, int w, int h, int nb, int[][] rgbs) throws Exception {
        FileOutputStream fout = new FileOutputStream(filename);

        fout.write( w / 256 ); fout.write( w % 256 );
        fout.write( h / 256 ); fout.write( h % 256 );
        fout.write( nb );

        for(int i=0; i < nb; i++) {
            for(int r=0; r < rgbs[i].length; r++) {
                int color = rgbs[i][r];

                int rr = (color >> 16) & 0xFF;
                int gg = (color >> 8) & 0xFF;
                int bb = (color) & 0xFF;

                color = color565(rr, gg, bb);

                fout.write(color/256);
                fout.write(color%256);
            }
        }

        fout.flush();
        fout.close();
    }

} 
