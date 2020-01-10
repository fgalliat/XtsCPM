import java.io.File;
import java.util.Arrays;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.GraphicsEnvironment;
import java.awt.font.FontRenderContext;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;

import javax.imageio.ImageIO;

public class Parser {

    public static void main(String[] args) throws Exception {
        char[] chars = new char[ (0x7F - 0x20) + 1 ];
        for(int i=0; i < chars.length; i++) {
            chars[i] = (char)(i + 0x20);
        }
        System.out.println("There will be "+ chars.length +" chars");
        String s = new String(chars);
        BufferedImage img = new Parser().stringToBufferedImage(s);

        ImageIO.write(img, "PNG", new File("font.png"));

        int nbChars = chars.length;
        int fontWidth = img.getWidth() / nbChars;
        int fontHeight = img.getHeight();

        System.out.println("Font is "+fontWidth+"x"+fontHeight+" px");

        int[] rgb = img.getRGB(0, 0, img.getWidth(), img.getHeight(), null, 0, img.getWidth() );

        int virtualFontHeight = 8;
        int[] font = new int[ nbChars*virtualFontHeight ];

        for(int i=0; i < nbChars; i++) {
            System.out.println( chars[i] );
            for(int y=0; y < fontHeight; y++) {
                String byteRepr = "";
                byteRepr += "000"; // left unused pixels
                for(int x=0; x < fontWidth; x++) {
                    int addr = ( y * img.getWidth() ) + (i*fontWidth)+x;
                    if ( rgb[addr] == 0 ) {
                        // Transparent Pixel (a=0)
                        System.out.print(" ");
                        byteRepr += "0";
                    } else {
                        // any color
                        System.out.print("*");
                        byteRepr += "1";
                    }
                }
                font[ (i * virtualFontHeight) + y ] = Integer.parseInt( byteRepr, 2 );
                System.out.println("");
            } 
            font[ (i * virtualFontHeight) + 6 ] = 0x00;
            font[ (i * virtualFontHeight) + 7 ] = 0x00; // last unused bottom lines
        }

        for(int i=0; i < font.length; i++) {
            int charNum = (i / virtualFontHeight);
            System.out.print( getHex(font[i]) );
            if (i < font.length-1) { System.out.print(", "); }
            if ( i % virtualFontHeight == (virtualFontHeight-1) ) { System.out.println("// "+getHex(0x20+charNum)+" "+((char)(0x20+charNum))+" ("+charNum+")"); }
        }
    }

    static String getHex(int v) {
        String tmp =Integer.toHexString(v).toUpperCase(); 
        return "0x"+(tmp.length() < 2 ? "0" : "")+tmp;
    }


    public BufferedImage stringToBufferedImage(String s) throws Exception {
        //First, we have to calculate the string's width and height
    
        BufferedImage img = new BufferedImage(1, 1, BufferedImage.TYPE_4BYTE_ABGR);
        Graphics g = img.getGraphics();
    
        //Set the font to be used when drawing the string

        GraphicsEnvironment ge = null;
        ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        ge.registerFont(Font.createFont(Font.TRUETYPE_FONT, this.getClass().getResourceAsStream("./pzim3x5.ttf")));

System.out.println( Arrays.asList( ge.getAvailableFontFamilyNames() ) );

        Font f = new Font("Pixelzim", Font.PLAIN, 8);
        // // Font f = Font.createFont(Font.TRUETYPE_FONT, this.getClass().getResourceAsStream("./pzim3x5.ttf"));
        g.setFont(f);
    
        //Get the string visual bounds
        FontRenderContext frc = g.getFontMetrics().getFontRenderContext();
        Rectangle2D rect = f.getStringBounds(s, frc);
        //Release resources
        g.dispose();
    
        //Then, we have to draw the string on the final image
    
        //Create a new image where to print the character
        img = new BufferedImage((int) Math.ceil(rect.getWidth()), (int) Math.ceil(rect.getHeight()), BufferedImage.TYPE_4BYTE_ABGR);
        System.out.println("Image will be "+ img.getWidth()+"x"+ img.getHeight()+" pixels");
        System.out.println("Image will be "+ (img.getWidth() / s.length() ) +"x"+ img.getHeight()+" char pixels");
        g = img.getGraphics();
        g.setColor(Color.black); //Otherwise the text would be white
        g.setFont(f);
    
        //Calculate x and y for that string
        FontMetrics fm = g.getFontMetrics();
        int x = 0;
        int y = fm.getAscent(); //getAscent() = baseline
        g.drawString(s, x, y);
    
        //Release resources
        g.dispose();
    
        //Return the image
        return img;
    }


}