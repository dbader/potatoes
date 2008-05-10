/**
 * Spezifikation des Prozesses für read und write
 * 
 */
 
CALL: write <FD, *buf, num_bytes, current_file_pos>
 
1. lade notwendigen Block in den block cache (s. read)

2. kopiere num_bytes von *buf nach [block cache head + current_file_pos]
                alles geschrieben oder Blockgrenze erreicht
                        schreibe Block auf HD
                nicht alles geschrieben und Blockgrenze erreicht
                        current block++
                        goto 1
                        
3. return TRUE                        
          
                        
