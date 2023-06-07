----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/23/2020 01:23:55 PM
-- Design Name: 
-- Module Name: clockdiv - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity clockdiv is
    Port ( clk : in STD_LOGIC;
           clk_out : out STD_LOGIC);
end clockdiv;

architecture Behavioral of clockdiv is
    signal counter : std_logic_vector(31 downto 0) := (others => '0');
    signal output : std_logic := '0';
begin
    clk_out <= output;
    
    adder : process(clk) begin
        if rising_edge(clk) then
            if unsigned(counter) = 50000 then
                output <= not output;
                counter <= (others => '0');
            else
                counter <= std_logic_vector( unsigned(counter) + 1 );
            end if; 
        end if;
    end process;
end Behavioral;
