library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity running_leds is
    Port ( nrst : in STD_LOGIC;
           CLK : in STD_LOGIC;
           dataout : out STD_LOGIC_VECTOR (7 downto 0));
end running_leds;

architecture Behavioral of running_leds is
    signal counter : unsigned(25 downto 0) := (others => '0');
    signal sig_LEDs : std_logic_vector(7 downto 0);
begin
    process(clk,nrst)
    begin
        if nrst = '0' then
            counter <= (others => '0');
            sig_LEDs <= "00000001";
        elsif rising_edge(clk) then
            if counter < 49999999 then
                counter <= counter + 1;
            else
                counter <= (others => '0');
                sig_LEDs <= sig_LEDs(6 downto 0 )&sig_LEDs(7);
            end if;
        end if;
    end process;
    
    dataout <= sig_LEDs;

end Behavioral;
