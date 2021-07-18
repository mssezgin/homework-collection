`timescale 1ns / 1ps

module kb(
    input K,
    input B,
    input clk,
    output reg Q
    );

    initial begin
        Q = 0;
    end

    always@(posedge clk)
    begin
        if (K == 1'b0 && B == 1'b0)
            Q <= ~Q;
        else if (K == 1'b0 && B == 1'b1)
            Q <= 1'b0;
        else if (K == 1'b1 && B == 1'b0)
            Q <= 1'b1;
        else
            Q <= Q;
    end

endmodule


module ic232(
    input A0,
    input A1,
    input A2,
    input clk,
    output Q0,
    output Q1,
    output Z
    );

    wire K0, B1;

    assign K0 = ~(A0 | A1) & (~A2);
    assign B1 = (~A0 | A1) ^ A2;
    kb kb_0(K0, A1, clk, Q0);
    kb kb_1(~A2, B1, clk, Q1);
    assign Z = ~(Q0 ^ Q1);

endmodule
