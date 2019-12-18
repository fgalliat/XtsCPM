public class KeyMapper {


    public static void main(String[] args) {
        process();
    }

    public static void process() {
        StringBuffer sb = new StringBuffer();

        sb.append("const char keyMap[2][128] = {\n");

        for(int lang = 0; lang < 2; lang ++) {
            sb.append(" {\n");
            
            for(int i=0; i < 128; i++) {
                if ( i % 32 == 0 ) { sb.append("   "); }
                if ( i >= 1 && i <= 26+1 ) { sb.append("   "); }
                sb.append( (i >= 32 && i != 127) ? "'"+((char)i)+"'" : i );
                sb.append(", ");
                
                if ( i == 0 ) { sb.append("\n"); }
                if ( i >= 1 && i <= 26 ) { 
                    sb.append("\t// Ctrl + "); 
                    sb.append( (char)(i+'A'-1) ); 
                    sb.append("\n"); 
                }

                if ( i % 32 == 31 ) { sb.append("\n"); }
            }
            
            sb.append(" },\n");
        }

        sb.append("}\n");

        System.out.println(sb.toString());
    }

}