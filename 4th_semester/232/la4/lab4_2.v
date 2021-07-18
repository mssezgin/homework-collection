`timescale 1ns / 1ps

module IntersectionSimulator(
    input [2:0] mode, // 1xx:display, 000:remA, 001:remB, 010:addA, 011:addB
    input [4:0] plateIn,
    input action,
    input clk,
    output reg  greenForA,
    output reg  greenForB,
    output reg 	rushHourWarning,
    output reg [3:0]  hour,
    output reg [5:0]  minute,	
    output reg [5:0]  second,
    output reg	am_pm, // 0:am, 1:pm
    output reg [4:0]  numOfCarsA,
    output reg [4:0]  numOfCarsB,
    output reg [6:0]  remainingTime,
    output reg [4:0]  blackListDisplay
    );

    integer i;
    reg greenWasAorB; // 0:A, 1:B
    reg [6:0] redDurationForA;
    reg [6:0] redDurationForB;
    reg [4:0] carsOnRoadAItr [0:1]; // [0]: head, [1]: tail
    reg [4:0] carsOnRoadA [0:29];
    reg [4:0] carsOnRoadBItr [0:1]; // [0]: head, [1]: tail
    reg [4:0] carsOnRoadB [0:29];
    reg [4:0] blackListItr [0:1];   // [0]: current, [1]: tail
    reg [4:0] blackList [0:29];

    // initialization
    initial begin
        greenForA = 1;
        greenForB = 0;
        rushHourWarning = 0;
        hour = 6;
        minute = 0;
        second = 0;
        am_pm = 0;
        numOfCarsA = 0;
        numOfCarsB = 0;
        remainingTime = 50;
        blackListDisplay = 0;

        // local variables
        greenWasAorB = 0;
        redDurationForA = 7'd40;
        redDurationForB = 7'd50;
        for (i = 0; i < 30; i = i + 1) begin
            carsOnRoadA [i] = 5'd0;
            carsOnRoadB [i] = 5'd0;
            blackList [i] = 5'd0;
        end
        carsOnRoadAItr [0] = 5'd0;
        carsOnRoadAItr [1] = 5'd0;
        carsOnRoadBItr [0] = 5'd0;
        carsOnRoadBItr [1] = 5'd0;
        blackListItr [0] = 5'd0;
        blackListItr [1] = 5'd0;
    end


    // action
    always@(posedge action)
    begin

        case (mode)

            3'b000  : begin
                if (greenForA == 0) begin
                    blackList [blackListItr [1]] <= carsOnRoadA [carsOnRoadAItr [0]];
                    blackListItr [1] <= blackListItr [1] + 1;
                end
                carsOnRoadA [carsOnRoadAItr [0]] <= 5'd0;
                carsOnRoadAItr [0] <= (carsOnRoadAItr [0] == 5'd29) ? 5'd0 : carsOnRoadAItr [0] + 1;
                numOfCarsA <= numOfCarsA - 1;
            end

            3'b001  : begin
                if (greenForB == 0) begin
                    blackList [blackListItr [1]] <= carsOnRoadB [carsOnRoadBItr [0]];
                    blackListItr [1] <= blackListItr [1] + 1;
                end
                carsOnRoadB [carsOnRoadBItr [0]] <= 5'd0;
                carsOnRoadBItr [0] <= (carsOnRoadBItr [0] == 5'd29) ? 5'd0 : carsOnRoadBItr [0] + 1;
                numOfCarsB <= numOfCarsB - 1;
            end

            3'b010  : begin
                carsOnRoadA [carsOnRoadAItr [1]] <= plateIn;
                carsOnRoadAItr [1] <= (carsOnRoadAItr [1] == 5'd29) ? 5'd0 : carsOnRoadAItr [1] + 1;
                numOfCarsA <= numOfCarsA + 1;
            end

            3'b011  : begin
                carsOnRoadB [carsOnRoadBItr [1]] <= plateIn;
                carsOnRoadBItr [1] <= (carsOnRoadBItr [1] == 5'd29) ? 5'd0 : carsOnRoadBItr [1] + 1;
                numOfCarsB <= numOfCarsB + 1;
            end

            default : ;

        endcase

    end


    // clk
    always@(posedge clk)
    begin

        // if (mode == 0xx)
        if ((mode & 3'b100) == 3'b000) begin

            blackListItr [0] <= 5'd0;

            // 12 hour clock
            if (second == 6'd59) begin
                second <= 6'd0;

                if (minute == 6'd59) begin
                    minute <= 6'd0;

                    if (hour == 4'd12)
                        hour <= 4'd1;
                    else
                        hour <= hour + 1;

                    if (hour == 4'd11) begin
                        am_pm <= ~am_pm;

                        if (am_pm == 1) begin // 11:59:59 pm, reset black list
                            blackListDisplay <= 5'd0;
                            blackListItr [0] <= 5'd0;
                            blackListItr [1] <= 5'd0;
                            for (i = 0; i < 30; i = i + 1)
                                blackList [i] <= 5'd0;
                        end
                    end
                end
                else
                    minute <= minute + 1;
            end
            else
                second <= second + 1;

            // remaining time
            if (remainingTime == 7'd0) begin

                if (greenWasAorB == 0) begin // A
                    remainingTime <= redDurationForA;
                    greenForB <= 1;

                    if (numOfCarsB < 5'd11) begin
                        redDurationForB <= (rushHourWarning == 0) ?
                            ((redDurationForB + 5 < 7'd80) ? redDurationForB + 5 : 7'd80):
                            ((redDurationForB + 5 < 7'd70) ? redDurationForB + 5 : 7'd70);
                    end
                    else if (numOfCarsB > 5'd19) begin
                        redDurationForB <= (rushHourWarning == 0) ?
                            ((redDurationForB - 5 > 7'd50) ? redDurationForB - 5 : 7'd50):
                            ((redDurationForB - 5 > 7'd40) ? redDurationForB - 5 : 7'd40);
                    end
                    else begin
                        redDurationForB <= (rushHourWarning == 0) ?
                            ((redDurationForB > 7'd80) ? 7'd80 : ((redDurationForB < 7'd50) ? 7'd50 : redDurationForB)):
                            ((redDurationForB > 7'd70) ? 7'd70 : ((redDurationForB < 7'd40) ? 7'd40 : redDurationForB));
                    end
                end
                else if (greenWasAorB == 1) begin // B
                    remainingTime <= redDurationForB;
                    greenForA <= 1;

                    if (numOfCarsA < 5'd11) begin
                        redDurationForA <= (rushHourWarning == 0) ?
                            ((redDurationForA + 5 < 7'd70) ? redDurationForA + 5 : 7'd70):
                            ((redDurationForA + 5 < 7'd60) ? redDurationForA + 5 : 7'd60);
                    end
                    else if (numOfCarsA > 5'd19) begin
                        redDurationForA <= (rushHourWarning == 0) ?
                            ((redDurationForA - 5 > 7'd40) ? redDurationForA - 5 : 7'd40):
                            ((redDurationForA - 5 > 7'd30) ? redDurationForA - 5 : 7'd30);
                    end
                    else begin
                        redDurationForA <= (rushHourWarning == 0) ?
                            ((redDurationForA > 7'd70) ? 7'd70 : ((redDurationForA < 7'd40) ? 7'd40 : redDurationForA)):
                            ((redDurationForA > 7'd60) ? 7'd60 : ((redDurationForA < 7'd30) ? 7'd30 : redDurationForA));
                    end
                end
                greenWasAorB <= ~greenWasAorB;
            end
            else
                remainingTime <= remainingTime - 1;

            if (remainingTime == 7'd1) begin
                if (greenForA == 1)
                    greenForA <= 0;
                else if (greenForB == 1)
                    greenForB <= 0;
            end

        end

        // if (mode == 1xx)
        else if ((mode & 3'b100) == 3'b100)
        begin

            blackListDisplay <= blackList [blackListItr [0]];
            blackListItr [0] <= (blackListItr [0] + 1 < blackListItr [1]) ? blackListItr [0] + 1: 5'd0;

        end

    end


    // rush hour warning
    always@(*) rushHourWarning <= ((am_pm == 0 && (hour == 4'd7 || hour == 4'd8)) || (am_pm == 1 && (hour == 4'd5 || hour == 4'd6))) ? 1 : 0;

endmodule
