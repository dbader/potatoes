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

4. aktualisiere size in m_inode mit size = max (size, (current_file_pos + num_bytes))
                        
3. return TRUE                        


/****************************************************************************************************/


CALL: read <FD, *buf, num_bytes/1(elementar), current_file_pos>

1. berechne zu lesenden Block aus current_file_pos

2. lade ggf. crt_ind_adr_block in den Speicher

3. ermittle Adresse zum Block

4. lade Block in block_cache

5. lese ab current_file_pos bis current_file_pos + num_bytes und kopiere nach *buf
                Blockgrenze erreicht
                        block_nr++
                        goto 2/3(optimiert)
                        
! current_file_pos wird bei return = true automatisch vom PM auf current_file_pos + num_bytes gesetzt

6. return (byte)*buf (Datenbyte)