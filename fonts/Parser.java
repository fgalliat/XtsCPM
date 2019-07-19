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
        char[] chars = new char[ 0x7F - 0x20 ];
        for(int i=0; i < chars.length; i++) {
            chars[i] = (char)(i + 0x20);
        }
        String s = new String(chars);
        BufferedImage img = new Parser().stringToBufferedImage(s);
        ImageIO.write(img, "PNG", new File("font.png"));
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