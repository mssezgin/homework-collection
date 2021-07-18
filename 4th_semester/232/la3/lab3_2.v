`timescale 1ns / 1ps

module QueueCounter(
    input [7:0] llmt,
    input [7:0] ulmt,
    input en,
    input mode, // 1:delete, 0:insert
    input clk,
    output reg [7:0] head,
    output reg [7:0] tail,
    output empty,
    output full);

    reg emptyreg;
    reg fullreg;

    initial
    begin
        emptyreg = 1;
        fullreg = 0;
    end

    initial
    begin
        head = llmt;
        tail = llmt;
    end

    always@(posedge clk)
    begin
        if (en == 1 && mode == 0 && fullreg == 0)
        begin
            if (tail == ulmt)
                tail = llmt;
            else
                tail = tail + 1;
            
            emptyreg = 0;
            fullreg = (head == tail) ? 1 : 0;
        end
        else if (en == 1 && mode == 1 && emptyreg == 0)
        begin
            if (head == ulmt)
                head = llmt;
            else
                head = head + 1;
            
            emptyreg = (head == tail) ? 1 : 0;
            fullreg = 0;
        end
    end

    assign empty = emptyreg;
    assign full = fullreg;

endmodule


module Numerator(
    input clinic, 
    input mode, 
    input en, 
    input clk,
    output [7:0] ann,
    output [7:0] print);

    wire [7:0] head0, tail0, head1, tail1;
    wire en0, en1, empty0, full0, empty1, full1;
    reg [7:0] annreg, printreg;

    QueueCounter room0(8'd5, 8'd9, en0, mode, clk, head0, tail0, empty0, full0);
    QueueCounter room1(8'd10, 8'd14, en1, mode, clk, head1, tail1, empty1, full1);

    assign en0 = en && ~clinic;
    assign en1 = en && clinic;

    always@(*)
        if (en == 1)
        begin
            annreg = (clinic ? head1 : head0);
            printreg = (clinic ? tail1 : tail0);
        end

    assign ann = annreg;
    assign print = printreg;

endmodule
