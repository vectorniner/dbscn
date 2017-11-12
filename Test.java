import javax.swing.*;
import java.util.*;
import java.io.*;
import java.awt.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

public class Test {

    //private static final String FILE = new File(args[0]);
    private static Point[] points;

    public static void main(final String[] args){
        try{
			File FILE = new File(args[0]);
            //final BufferedReader br = new BufferedReader(new FileReader(new File(FILE)));
            points = new Point[Integer.parseInt(args[1])];
            Scanner input = new Scanner(FILE);
			int i = 0;
            int xMax = 0;
            int yMax = 0;
            while(input.hasNext())
			{
					//line = input.nextLine();
					//System.out.println(line);
					final int x = Integer.parseInt(input.next());
					final int y = Integer.parseInt(input.next());
					xMax = Math.max(x, xMax);
					yMax = Math.max(y, yMax);
					System.out.println("x="+x+", y="+y);
					points[i++] = new Point(x, y);
			}
			input.close();
/*			
			while(br.ready()){
                final String[] split = br.readLine().split("\u0009");
                final int x = Integer.parseInt(split[0]);
                final int y = Integer.parseInt(split[1]);
                xMax = Math.max(x, xMax);
                yMax = Math.max(y, yMax);
                points[i++] = new Point(x, y);
            }
*/
			final JFrame frame = new JFrame("Point Data Rendering");
            final Panel panel = new Panel();
            panel.setPreferredSize(new Dimension(xMax + 10, yMax + 10));
            frame.setContentPane(panel);
            frame.pack();
            frame.setVisible(true);
            frame.repaint();
        } catch (final Exception e){
            e.printStackTrace();
        }
    }

    public static class Panel extends JPanel {

        @Override
        public void paintComponent(final Graphics g){
            g.setColor(Color.RED);
            for(final Point p : points){
                g.fillRect((int) p.getX(), (int) p.getY(), 2, 2);
            }
        }

    }

}