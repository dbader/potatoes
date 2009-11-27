/**
 * Spezifikation des Prozesses für open und close
 * 
 */

CALL: open <"/Pfadkomponente 1/Pfadkomponente 2/.../name">

1. Existiert eine Datei mit gleichem Namen in der globalen FILP?
                yes:    return Fehlermeldung oder Deskriptor
                no:     continue
                
2. current_dir = "/"
                durchsuche current_dir nach nächster Pfadkomponente
                        gefunden && Pfadkomponente = name/letzte Pfadkomponente:
                                goto 3.
                        gefunden && Pfadkomponente != letzte Pfadkomponente:
                                current_dir = Pfadkomponente
                                goto 2. (rekursiv)
                        nicht gefunden 
                                return FALSE

3. Lade Ziel-inode-Block nach inode-Tabelle
                suche dabei linear nach freier Zeile in inode-Tabelle
                        freien Platz/freie Zeile gefunden
                                Zeilennummer = Deskriptornummer
                                goto 4.
                        keinen freien Platz gefunden
                                return Fehlermeldung
                                
4. registriere inode in globaler FILP
                
5. registriere file descriptor der inode in Prozess-FILP mit current_file_pos = 0

RETURN descriptor



/****************************************************************************************************/



CALL: close <file descriptor>

1. m_inode in d_inode konvertieren und inode auf HD schreiben

2. inode aus inode-Tabelle entfernen

3. inode/FD aus globaler FILP entfernen

4. return TRUE -> FS aus Prozess-FILP wird entfernt
