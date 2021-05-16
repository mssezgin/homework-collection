module PE4 where

import Data.Maybe -- up to you if you want to use it or not

-- Generic DictTree definition with two type arguments
data DictTree k v = Node [(k, DictTree k v)] | Leaf v deriving Show

-- Lightweight Char wrapper as a 'safe' Digit type
newtype Digit = Digit Char deriving (Show, Eq, Ord) -- derive equality and comparison too!

-- Type aliases
type DigitTree = DictTree Digit String
type PhoneNumber = [Digit]


---------------------------------------------------------------------------------------------
------------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
--------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
--------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
---------------------------------------------------------------------------------------------


----------
-- Part I:
-- Some Maybe fun! Basic practice with an existing custom datatype.

-- toDigit: Safely convert a character to a digit
toDigit :: Char -> Maybe Digit
toDigit d = if ('0' <= d && d <= '9') then Just (Digit d)
                                      else Nothing


-- toDigits: Safely convert a bunch of characters to a list of digits.
--           Particularly, an empty string should fail.
toDigits :: String -> Maybe PhoneNumber
toDigits [] = Nothing
toDigits (d:ds) =   let
                        firstPN = toDigit d
                    in
                        if (firstPN == Nothing) then
                            Nothing
                        else if (ds == []) then
                            Just [Digit d]
                        else
                            let
                                restPN = toDigits ds
                            in
                                if restPN == Nothing then
                                    Nothing
                                else
                                    Just ((Digit d):(fromJust restPN))

-- toDigits (d:[]) = if (toDigit d) == Nothing then Nothing
--                                             else Just ([Digit d])
-- toDigits (d:ds) = let
--                     firstPN = toDigit d
--                     restPN = toDigits ds
--                   in
--                     if (firstPN == Nothing || restPN == Nothing) then Nothing
--                                                                  else Just ((Digit d):(fromJust restPN))


-----------
-- Part II:
-- Some phonebook business.

-- numContacts: Count the number of contacts in the phonebook...
numContacts :: DigitTree -> Int
numContacts (Leaf _) = 1
numContacts (Node nodes) =  foldl (+) 0 (map sumOfNode nodes)
                                where
                                    sumOfNode node = numContacts (snd node)
--                         foldl (+) 0 sumOfNodes
--                          where
--                              sumOfNodes = [numContacts (snd node) | node <- nodes]

    
-- getContacts: Generate the contacts and their phone numbers in order given a tree. 
getContacts :: DigitTree -> [(PhoneNumber, String)]
getContacts book = preGetContacts [] book


preGetContacts :: PhoneNumber -> DigitTree -> [(PhoneNumber, String)]
preGetContacts prefix (Leaf name) = [(prefix, name)]
preGetContacts _      (Node []) = []
preGetContacts prefix (Node (node:nodes)) = (preGetContacts (prefix ++ [fst node]) (snd node)) ++ preGetContacts prefix (Node nodes)


-- autocomplete: Create an autocomplete list of contacts given a prefix
-- e.g. autocomplete "32" areaCodes -> 
--      [([Digit '2'], "Adana"), ([Digit '6'], "Hatay"), ([Digit '8'], "Osmaniye")]
autocomplete :: String -> DigitTree -> [(PhoneNumber, String)]
autocomplete [] _ = []
autocomplete strPrefix book =   let
                                    prefix = toDigits strPrefix
                                in
                                    if prefix == Nothing then [] else preAutocomplete (fromJust prefix) book


preAutocomplete :: PhoneNumber -> DigitTree -> [(PhoneNumber, String)]
preAutocomplete [] book = getContacts book
preAutocomplete _ (Leaf _) = []
preAutocomplete _ (Node []) = []
preAutocomplete (p:ps) (Node (node:nodes)) = let
                                                q = fst node
                                             in
                                                if p < q then
                                                    []
                                                else if p == q then
                                                    preAutocomplete ps (snd node)
                                                else
                                                    preAutocomplete (p:ps) (Node nodes)


-----------
-- Example Trees
-- Two example trees to play around with, including THE exampleTree from the text. 
-- Feel free to delete these or change their names or whatever!

exampleTree :: DigitTree
exampleTree = Node [
    (Digit '1', Node [
        (Digit '3', Node [
            (Digit '7', Node [
                (Digit '8', Leaf "Jones")])]),
        (Digit '5', Leaf "Steele"),
        (Digit '9', Node [
            (Digit '1', Leaf "Marlow"),
            (Digit '2', Node [
                (Digit '3', Leaf "Stewart")])])]),
    (Digit '3', Leaf "Church"),
    (Digit '7', Node [
        (Digit '2', Leaf "Curry"),
        (Digit '7', Leaf "Hughes")])]

areaCodes :: DigitTree
areaCodes = Node [
    (Digit '3', Node [
        (Digit '1', Node [
            (Digit '2', Leaf "Ankara")]),
        (Digit '2', Node [
            (Digit '2', Leaf "Adana"),
            (Digit '6', Leaf "Hatay"),
            (Digit '8', Leaf "Osmaniye")])]),
    (Digit '4', Node [
        (Digit '6', Node [
            (Digit '6', Leaf "Artvin")])])]

