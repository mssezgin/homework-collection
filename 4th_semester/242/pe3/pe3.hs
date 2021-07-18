module PE3 where

data Cell = SpaceCraft Int | Sand | Rock Int | Pit deriving (Eq, Read, Show)

type Grid = [[Cell]]
type Coordinate = (Int, Int)

data Move = North | East | South | West | PickUp | PutDown deriving (Eq, Read, Show)

data Robot = Robot { name :: String,
                     location :: Coordinate,
                     capacity :: Int,
                     energy :: Int,
                     storage :: Int } deriving (Read, Show)

-------------------------------------------------------------------------------------------
--------------------------------- DO NOT CHANGE ABOVE -------------------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------
-------------------------------------- PART I ---------------------------------------------

isInGrid :: Grid -> Coordinate -> Bool
isInGrid grid coor = 0 <= x && x < xmax && 0 <= y && y < ymax
                    where
                        x = fst coor
                        y = snd coor
                        xmax = length (head grid)
                        ymax = length grid

-------------------------------------------------------------------------------------------

totalCount :: Grid -> Int
totalCount grid = foldl totalCountRow 0 grid


totalCountRow :: Int -> [Cell] -> Int
totalCountRow i gridRow = foldl countRocks i gridRow
                            where
                                countRocks x (Rock y) = x + y
                                countRocks x _ = x

-------------------------------------------------------------------------------------------

coordinatesOfPits :: Grid -> [Coordinate]
coordinatesOfPits grid = recurCoordinatesOfPits grid 0


recurCoordinatesOfPits :: Grid -> Int -> [Coordinate]
recurCoordinatesOfPits ([]:restEmpties) _ = []
recurCoordinatesOfPits grid x = coordInColumnX ++ recurCoordinatesOfPits newGrid (x + 1)
                                    where
                                        columnwise = coordinatesOfPitsByColumn grid x 0
                                        newGrid = fst columnwise
                                        coordInColumnX = snd columnwise


coordinatesOfPitsByColumn :: Grid -> Int -> Int -> (Grid, [Coordinate])
coordinatesOfPitsByColumn [] _ _ = ([], [])
coordinatesOfPitsByColumn (row:restRows) x y = (restOfRow:restOfNewGrid, coordInCellXY ++ coordInRestOfColumnX)
                                                where
                                                    rowwise = coordinatesOfPitsByRow row x y
                                                    restOfRow = fst rowwise
                                                    coordInCellXY = snd rowwise
                                                    rowwiseRest = coordinatesOfPitsByColumn restRows x (y + 1)
                                                    restOfNewGrid = fst rowwiseRest
                                                    coordInRestOfColumnX = snd rowwiseRest


coordinatesOfPitsByRow :: [Cell] -> Int -> Int -> ([Cell], [Coordinate])
coordinatesOfPitsByRow [] _ _ = ([], [])
coordinatesOfPitsByRow (cell:restCells) x y = case cell of Pit -> (restCells, (x, y):[])
                                                           _   -> (restCells, [])

-------------------------------------------------------------------------------------------

tracePath :: Grid -> Robot -> [Move] -> [Coordinate]
tracePath grid robot moves = tracePathL grid robot moves (length (head grid)) (length grid)


tracePathL _ _ [] _ _ = []
tracePathL grid robot (move:restMoves) xmax ymax = let lcn = location robot
                                                       cpt = capacity robot
                                                       eng = energy robot
                                                       stg = storage robot
                                                   in
                                                       [] -- case move of PickUp 

------------------------------------- PART II ----------------------------------------------

energiseRobots :: Grid -> [Robot] -> [Robot]
energiseRobots grid robots = robots

-------------------------------------------------------------------------------------------

applyMoves :: Grid -> Robot -> [Move] -> (Grid, Robot)
applyMoves grid robot moves = applyMovesL grid robot moves (coordinatesOfPits grid)


applyMovesL grid robot [] _ = (grid, robot)
applyMovesL grid robot (move:restMoves) pits = applyMovesL (fst step) (snd step) restMoves pits
                                                where
                                                    step = applyMove grid robot move pits


applyMove grid robot move pits = (newgrid, Robot {name = (name robot), location = newlcn, capacity = cpt, energy = neweng, storage = newstg})
                                    where
                                        lcn = location robot
                                        cpt = capacity robot
                                        eng = energy robot
                                        stg = storage robot
                                        inPit = lcn `elem` pits
                                        newpossibleeng = case move of PickUp  -> (eng - 5)
                                                                      PutDown -> (eng - 3)
                                                                      _ -> (eng - 1)
                                        neweng = max 0 newpossibleeng
                                        newlcn = let newpossiblelcn = case move of North -> (fst lcn, (snd lcn) - 1)
                                                                                   East  -> ((fst lcn) + 1, snd lcn)
                                                                                   South -> (fst lcn, (snd lcn) + 1)
                                                                                   West  -> ((fst lcn) - 1, snd lcn)
                                                                                   _ -> lcn
                                                     exceed = not (isInGrid grid newpossiblelcn)
                                                 in if inPit || exceed || (newpossibleeng < 0) then lcn else newpossiblelcn
                                        newstg = case move of PickUp  -> if inPit || (newpossibleeng < 0) || (stg >= cpt) || (((\(Rock r) -> r) (grid !! (snd lcn) !! (fst lcn))) <= 0) then stg else (stg + 1)
                                                              PutDown -> if inPit || (newpossibleeng < 0) then stg else (stg - 1)
                                                              _ -> stg
                                        newgrid = case move of PickUp  -> if inPit || (newpossibleeng < 0) || (stg >= cpt) || (((\(Rock r) -> r) (grid !! (snd lcn) !! (fst lcn))) <= 0) then grid
                                                                                                                                                                                         else ((take y grid) ++ [(take x row) ++ [Rock (((\(Rock r) -> r) (row !! x)) - 1)] ++ (drop (x + 1) row)] ++ (drop (y + 1) grid))
                                                                                                                                                                                                where
                                                                                                                                                                                                    x = fst lcn
                                                                                                                                                                                                    y = snd lcn
                                                                                                                                                                                                    row = (grid !! y)
                                                               PutDown -> if inPit || (newpossibleeng < 0) then grid
                                                                                                           else ((take y grid) ++ [(take x row) ++ [SpaceCraft (((\(SpaceCraft r) -> r) (row !! x) + 1))] ++ (drop (x + 1) row)] ++ (drop (y + 1) grid))
                                                                                                                    where
                                                                                                                        x = fst lcn
                                                                                                                        y = snd lcn
                                                                                                                        row = (grid !! y)
                                                               _ -> grid
