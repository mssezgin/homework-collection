module PE2 where

---------------------------------------------------------------------------------------------
------------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
--------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
--------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
---------------------------------------------------------------------------------------------

-- Note: undefined is a value that causes an error when evaluated. Replace it with
-- a viable definition! Name your arguments as you like by changing the holes: _


--------------------------
-- Part I: Time to inf up!

-- naturals: The infinite list of natural numbers. That's it!
naturals :: [Integer]
naturals = [0..]

-- interleave: Interleave two lists, cutting off on the shorter list.
interleave :: [a] -> [a] -> [a]
interleave [] _ = []
interleave _ [] = []
interleave (x:xs) (y:ys) = x:y:(interleave xs ys)

-- integers: The infinite list of integers. Ordered as [0, -1, 1, -2, 2, -3, 3, -4, 4...].
integers :: [Integer]
integers = interleave [0..] [-1,-2..]


--------------------------------
-- Part II: SJSON Prettification

-- splitOn: Split string on first occurence of character.
splitOn :: Char -> String -> (String, String)
splitOn _ "" = ("", "")
splitOn c (x:xs) | c == x = ("", xs)
                 | otherwise = (x:(fst rest), snd rest)
                                where rest = splitOn c xs

-- tokenizeS: Transform an SJSON string into a list of tokens.
tokenizeS :: String -> [String]
tokenizeS sjson = tokenizeIndex sjson 0

tokenizeIndex "" _ = []
tokenizeIndex (j:js) x = case j of ' '  -> tokenizeIndex js x
                                   '\n' -> tokenizeIndex js x
                                   '\t' -> tokenizeIndex js x
                                   '{'  -> "{":(tokenizeIndex js (x+1))
                                   '}'  -> if x == 1 then "}":[]
                                                     else "}":(tokenizeIndex js (x-1))
                                   '\'' -> (fst dequote):(tokenizeIndex (snd dequote) x)
                                                where dequote = splitOn '\'' js
                                   ':'  -> ":":(tokenizeIndex js x)
                                   ','  -> ",":(tokenizeIndex js x)

-- prettifyS: Prettify SJSON, better tokenize first!
prettifyS :: String -> String
prettifyS sjson = prettifyIndent (tokenizeS sjson) 0


prettifyIndent [] i = ""
prettifyIndent (j:js) i = case j of "{" -> '{':'\n':(take (4 * (i + 1)) (repeat ' ')) ++ (prettifyIndent js (i + 1))
                                    "}" -> '\n':(take (4 * (i - 1)) (repeat ' ')) ++ '}':(prettifyIndent js (i - 1))
                                    ":" -> ':':' ':(prettifyIndent js i)
                                    "," -> ',':'\n':(take (4 * i) (repeat ' ')) ++ (prettifyIndent js i)
                                    j   -> '\'':j ++ '\'':(prettifyIndent js i)
 

-- Good luck to you, friend and colleague!
