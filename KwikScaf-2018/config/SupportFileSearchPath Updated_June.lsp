;This sets a reference to the files portion of the acad preferences
(setq *files* (vla-get-files (vla-get-preferences (vlax-get-acad-object))))

;This Gets the supportpaths in one string
(setq searchpath (vla-get-supportpath *files*))

;Searches if the path is already in the supportpaths, 
;if not add the path to the string, using the ";" as a separator
(if (= (wcmatch searchpath "*C:\\Program Files\\KWIKSCAF-2013-64 UNPROTECTED\\Template*") nil)
(setq searchpath(strcat searchpath ";C:\\Program Files\\KWIKSCAF-2013-64 UNPROTECTED\\Template")))

;Searches if the path is already in the supportpaths,
;if not add the path to the string, using the ";" as a separator 
(if (= (wcmatch searchpath "*C:\\Program Files\\KwikScaf-2013-64\\Icons *") nil)
(setq searchpath(strcat searchpath ";C:\\Program Files\\KwikScaf-2013-64\\Icons ")))

;Searches if the path is already in the supportpaths, 
;if not add the path to the string, using the ";" as a separator
(if (= (wcmatch searchpath "*C:\\Program Files\\KWIKSCAF-2013-64 UNPROTECTED\\Icons*") nil)
(setq searchpath(strcat searchpath ";C:\\Program Files\\KWIKSCAF-2013-64 UNPROTECTED\\Icons")))

;write the new string of paths in the windows registry
(vla-put-supportpath *files* searchpath)