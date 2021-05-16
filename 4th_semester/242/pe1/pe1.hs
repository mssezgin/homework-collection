module PE1 where

import Text.Printf


-- This function takes a Double and rounds it to 2 decimal places as requested in the PE --
getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x

-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

convertTL :: Double -> String -> Double
convertTL money currency
    | currency == "USD" = getRounded (money / 8.18)
    | currency == "EUR" = getRounded (money / 9.62)
    | currency == "BTC" = getRounded (money / 473497.31)
    | otherwise         = 0

-------------------------------------------------------------------------------------------

countOnWatch :: [String] -> String -> Int -> Int
countOnWatch schedule employee days = countEmp (take days schedule) employee
    where
        countEmp [] y = 0
        countEmp (x:xs) y = (if x == y then 1 else 0) + (countEmp xs y)

-------------------------------------------------------------------------------------------

encrypt :: Int -> Int
encrypt x = encryptE x 3

encryptE x 0 = encryptDigit x
encryptE x e = (encryptDigit (x `div` 10^e)) * 10^e + encryptE (x `mod` 10^e) (e - 1)

encryptDigit x
    | (x `mod` 3) == 0 = (x - 1) `mod` 10
    | (x `mod` 4) == 0 = (x * 2) `mod` 10
    | (x `mod` 5) == 0 = (x + 3) `mod` 10
    | otherwise        = (x + 4) `mod` 10

-------------------------------------------------------------------------------------------

compoundInterests :: [(Double, Int)] -> [Double]
compoundInterests investments = [ getRounded (totalMoney m (interestRate m y) y) | (m, y) <- investments]

interestRate m y = if y >= 2 then (if m >= 10000 then 0.115 else 0.095)
                             else (if m >= 10000 then 0.105 else 0.09)

totalMoney p r n = p * (1 + r / 12) ^ (12 * n)
