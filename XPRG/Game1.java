import java.io.*;
import java.util.*;

public class Game1 {


    public static void main(String[] args) throws Exception {
        new Game1().process("GAME1.TXT");
    }

    // =================================
    public static class SceneEventCap {
        protected String capName;
        protected int id;

        public static SceneEventCap parse(String token) throws Exception {
            String[] tokens = token.split("\\:");
            SceneEventCap cap = new SceneEventCap();
            cap.capName = tokens[0];
            cap.id = Integer.parseInt(tokens[1]);
            return cap;
        }

        public String toString() {
            return ""+ capName+" => "+ id +"";
        }
    }

    public static class SceneEvent {
        protected int id;
        protected int newId = -1;

        protected String pakFile;
        protected int pakNum;
        protected String placeName;
        protected List<String> caracters;
        protected String text;
        protected List<SceneEventCap> caps = new ArrayList<SceneEventCap>();

        public static SceneEvent parse(String line) throws Exception {
            if ( line == null || line.isEmpty() ) { return null; }
            line = line.trim();
            String[] columns = line.split("\\|");

            // System.out.print( columns[0]+" $|$ "+columns[1]+" $|$ "+columns[2]+" $|$ "+columns[3]+" $|$ "+columns[4]+" $|$ "+columns[5]+" $|$ "+columns[6] );
            // System.out.println();

            SceneEvent evt = new SceneEvent();
            evt.id = Integer.parseInt( columns[0] );
            evt.pakFile = columns[1];
            evt.pakNum = Integer.parseInt( columns[2] );
            evt.placeName = columns[3];
            evt.caracters = Arrays.asList( columns[4].split("\\,") );
            evt.text = columns[5];
            String[] caps = columns[6].split("\\,");
            for(String cap : caps) {
                evt.caps.add( SceneEventCap.parse(cap) );
            }

            return evt;
        }

        public String toString() {
            return "[ "+placeName+" ] {"+ caracters +"} >>"+text+"<< ("+ caps +")";
        }

        public String toCode() {
            return "placeNam[ "+(newId+1)+" ] := '"+escapeStr(placeName)+"'; \n"+
            "pakNam[ "+(newId+1)+" ] := '"+ pakFile +"'; \n"+
            "pakNum[ "+(newId+1)+" ] := "+ pakNum +"; \n"+
            "text[ "+(newId+1)+" ] := '"+escapeStr(text)+"'; \n"/*+
                   "' {"+ caracters +"} >>"+text+"<< ("+ caps +")"*/;
        }
    }
    
    static String escapeStr(String str) {
        return str.replace('\'', ' ');
    }

    static List<SceneEvent> evts = new ArrayList<SceneEvent>();
    public static SceneEvent getSceneByDataId(int dataId) {
        for(SceneEvent evt : evts) {
            if ( evt.id == dataId ) {
                return evt;
            }
        }
        return null;
    }
    public static SceneEvent getSceneByIndex(int index) {
        return evts.get(index);
    }

    // =================================

    public void process(String dataFileName) throws Exception {
        File f = new File( dataFileName );
        BufferedReader reader = new BufferedReader( new FileReader( f ) );

        String line;
        evts.clear();
        int cpt = 0;
        while((line = reader.readLine()) != null) {
            SceneEvent evt = SceneEvent.parse(line);
            if (evt == null) { continue; }
            evt.newId = cpt++;
            evts.add(evt);
            System.out.println( evt.toCode() );
        }

        reader.close();
    }

}