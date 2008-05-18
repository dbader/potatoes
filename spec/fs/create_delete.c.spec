/**
 * Spezifikation des Prozesses für create und delete
 * 
 */

CALL: create "/dir_1/.../dir_n/file_name"

1. (rek.) durchsuchen ab "/" nach Zielordner

1.1 lese Ordner in block_cache
1.2 scanne Einträge

2. existiert eine Datei mit gleichem Namen in gleichem dir_n?
                yes: return Fehlermeldung/false
                no: goto 3.

3. erzeuge neue d_inode

4. finde freien Block in der Nähe des Verzeichnisblocks
                existiert nicht: return false

5. schreibe inode in Block

6. registriere inode (blocknr, name) in Verzeichnis (lineare Suche nach freiem Eintrag --> alte Lücken wd. gefüllt)
                kein freier Eintrag mehr vorhanden: return false
                

return true


/****************************************************************************************************/


CALL: delete "/dir_1/.../dir_n/file_name"

1. lese inode in block_cache

2. ermittle successiv alle block_adr != null aus der inode
2.1 lade ggf. indirekten Adressblock nach crt_ind_adr_block
2.2 löschen Flag aus bmap

3. lösche Verzeichniseintrag

4. 

return true