import java.io.*;
import java.util.*;

public class Game1 {

    // Offset des tableaux pour TurboPascal
    static final int TP_ARRY_OFFSET = 1;

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

        public String toCode() {
            return ""+ capName+":"+ (getSceneByDataId(id).newId + TP_ARRY_OFFSET ) +"";
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

        // caps is a reserved word .....
        public String toCode() {
            String spaces = "  ";
            return spaces+"placeNam[ "+(newId+TP_ARRY_OFFSET)+" ] := '"+escapeStr(placeName)+"'; \r\n"+
            spaces+"pakNam[ "+(newId+TP_ARRY_OFFSET)+" ] := '"+ pakFile +"'; \r\n"+
            spaces+"pakNum[ "+(newId+TP_ARRY_OFFSET)+" ] := "+ pakNum +"; \r\n"+
            spaces+"txt[ "+(newId+TP_ARRY_OFFSET)+" ] := '"+escapeStr(text)+"'; \r\n"+
            spaces+"caract[ "+(newId+TP_ARRY_OFFSET)+" ] := '"+escapeStr( join( caracters ) )+"'; \r\n"+
            spaces+"capas[ "+(newId+TP_ARRY_OFFSET)+" ] := '"+escapeStr( joinCap( caps ) )+"'; \r\n";
        }

        public static String toStruct() {
            String spaces = "  ";
            return spaces+"placeNam : array[1..nbScene] of string[32]; \r\n"+
                   spaces+"pakNam   : array[1..nbScene] of string[8]; \r\n"+
                   spaces+"pakNum   : array[1..nbScene] of integer; \r\n"+
                   spaces+"txt      : array[1..nbScene] of string[255]; \r\n"+
                   spaces+"caract   : array[1..nbScene] of string[32]; \r\n"+
                   spaces+"capas    : array[1..nbScene] of string[32]; \r\n";
                // "";
        }
    }
    
    static String joinCap(List<SceneEventCap> array) {
        String ret = "";
        for(SceneEventCap str : array) {
            ret += ""+str.toCode().trim()+",";
        }
        if (ret.length() > 0)  ret = ret.substring(0, ret.length()-1);
        ret = ret.trim();
        return ret;
    }

    static String join(List<String> array) {
        String ret = "";
        for(String str : array) {
            ret += ""+str+",";
        }
        if (ret.length() > 0)  ret = ret.substring(0, ret.length()-1);
        ret = ret.trim();
        return ret;
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

    protected void writeToFile(PrintStream out, String line) throws Exception {
        System.out.println(line);
        out.print(line+"\r\n");
    }

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
            System.out.println( evt.toString() );
        }
        reader.close();

        PrintStream fout = new PrintStream( new FileOutputStream("./GAME_S.INC") );

        writeToFile(fout, "{.PA}");
        writeToFile(fout, "{ Xtase Adventure Game1 }");
        writeToFile(fout, "{ Xtase - fgalliat @ Jul 2019 }");
        writeToFile(fout, "");

        writeToFile(fout, "const");
        writeToFile(fout, "   nbScene="+evts.size()+";");
        writeToFile(fout, "var ");
        writeToFile(fout,  SceneEvent.toStruct() );
        writeToFile(fout, "");

        writeToFile(fout, "procedure initScenes;");
        writeToFile(fout, "begin");
        // must be done after all ID <-> newID are assigned
        for(SceneEvent evt : evts) {
           writeToFile(fout,  evt.toCode() );
        }
        writeToFile(fout, "end;");

        fout.flush();
        fout.close();

    }

}