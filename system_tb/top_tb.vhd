library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use IEEE.STD_LOGIC_TEXTIO.ALL;
use STD.TEXTIO.ALL;

entity design_top_tb is
end entity design_top_tb;

architecture rtl of design_top_tb is

	function clogb2 (bit_depth : integer) return integer is            
        variable depth  : integer := bit_depth;                               
        variable count  : integer := 1;                                       
    begin                                                                   
         for clogb2 in 1 to bit_depth loop  -- Works for up to 32 bit integers
         if (bit_depth <= 2) then                                           
           count := 1;                                                      
         else                                                               
           if(depth <= 1) then                                              
               count := count;                                                
             else                                                             
               depth := depth / 2;                                            
             count := count + 1;                                            
             end if;                                                          
           end if;                                                            
      end loop;                                                             
      return(count);        	                                              
    end;   
    
    constant period_time            : time    := 83333 ps;
    constant TREES_ADDR             : integer := 16#20800#;
    constant FEATURES_ADDR          : integer := 16#40000#;
    constant CNTR_N_FEATURES        : integer := 16#00010#;
    constant CONTROL_ADDR           : integer := 16#00000#;
    constant PEDICTION_ADDR         : integer := 16#60000#;
    constant n_trees                : integer := 32;
    constant tree_size              : integer := 256;
    constant max_process_trees      : integer := 256;
    constant burst_size             : integer := 256;
    constant n_features             : integer := 1024;
    constant features_size          : integer := 33;
    constant one_float              : std_logic_vector(31 downto 0) := x"3f800000";



    signal   finished    : std_logic := '0';
    
    signal clk :        std_logic;
    signal nrst :       std_logic;
    
    signal m00_axi_init_axi_txn : std_logic;
    signal m00_axi_init_axi_rxn : std_logic;
    signal m00_axi_txn_ready : std_logic;
    signal m00_axi_rxn_ready : std_logic;
    signal test_wdata : std_logic_vector(31 downto 0);
    signal test_awaddr : std_logic_vector(31 downto 0);
    signal test_araddr : std_logic_vector(31 downto 0);
    signal test_rdata : std_logic_vector(31 downto 0) := (others => '0');
    signal read_status : std_logic_vector(31 downto 0) := (others => '0');

    
    signal INIT_AXI_TXN : std_logic := '0';
    signal INIT_AXI_RXN : std_logic := '0';
    signal BASE_AWADDR : std_logic_vector ( 31 downto 0 );
    signal BASE_ARADDR : std_logic_vector ( 31 downto 0 );
    signal N_BURSTs_RW : STD_LOGIC_VECTOR ( 31 downto 0 );
    signal C_M_AXI_BURST_LEN_PORT : STD_LOGIC_VECTOR ( 7 downto 0 );
    signal M_AXI_WDATA_TB : STD_LOGIC_VECTOR ( 31 downto 0 );
    signal M_AXI_RDATA_TB : STD_LOGIC_VECTOR ( 31 downto 0 );
    signal M_AXI_WSTRB_TB : STD_LOGIC_VECTOR ( 3 downto 0 );
    signal WRITE_VALID : STD_LOGIC;
    signal READ_VALID : STD_LOGIC;
    signal SYSTEM_IDLE : STD_LOGIC;

    signal data_filled : STD_LOGIC := '0';

    type word64  is array (0 to 8191) of std_logic_vector(63 downto 0);
        signal word64_array  : word64 ;

    type word32  is array (0 to features_size  * n_features) of std_logic_vector(31 downto 0);
        signal word32_array         : word32 ;
        signal word32_features      : word32 ;
        signal word32_predictions    : word32 ;
        signal word32_results       : word32 ;

    component design_1_wrapper is
        port (
        ------------- debug from tb ----------
        --------------  AXI LITE -------------
            test_wdata  : in std_logic_vector(31 downto 0);
            test_awaddr : in std_logic_vector(31 downto 0);
            test_araddr : in std_logic_vector(31 downto 0);
            test_rdata  : out std_logic_vector(31 downto 0);
            m00_axi_init_axi_txn : in STD_LOGIC;
            m00_axi_init_axi_rxn : in STD_LOGIC;
            m00_axi_txn_ready : out STD_LOGIC;
            m00_axi_rxn_ready : out STD_LOGIC;
        --------------  AXI FULL ------------
            INIT_AXI_TXN : in STD_LOGIC;
            INIT_AXI_RXN : in STD_LOGIC;
            BASE_AWADDR : in STD_LOGIC_VECTOR ( 31 downto 0 );
            BASE_ARADDR : in STD_LOGIC_VECTOR ( 31 downto 0 );
            N_BURSTs_RW : in STD_LOGIC_VECTOR ( 31 downto 0 );
            M_AXI_WDATA_TB : in STD_LOGIC_VECTOR ( 31 downto 0 );
            M_AXI_RDATA_TB : out STD_LOGIC_VECTOR ( 31 downto 0 );
            WRITE_VALID : out STD_LOGIC;
            READ_VALID : out STD_LOGIC;
            M_AXI_WSTRB_TB : in STD_LOGIC_VECTOR ( 3 downto 0 );
            SYSTEM_IDLE : out STD_LOGIC;
        -------------------------------------
            
            clk : in STD_LOGIC;
            nrst : in STD_LOGIC

        );
    end component design_1_wrapper;


    procedure send_data_lite(
        constant data                   : integer;
        constant addr                   : integer;
        signal m00_axi_init_axi_txn0    : out std_logic;
        signal m00_axi_txn_ready0       : in  std_logic;
        signal test_wdata0              : out std_logic_vector ( 31 downto 0 );
        signal test_awaddr0             : out std_logic_vector ( 31 downto 0 )
    ) is
    begin
        wait until rising_edge(clk);
        m00_axi_init_axi_txn0 <= '1';
        test_wdata0 <= std_logic_vector(to_unsigned(data, 32));
        test_awaddr0 <= std_logic_vector(to_unsigned(addr, 32));
        wait until rising_edge(clk);
        m00_axi_init_axi_txn0 <= '0';
        wait until m00_axi_txn_ready0 = '1';
        wait until rising_edge(clk);
    end procedure;

    
    procedure send_data_full(
        constant wr_addr        : integer;
        constant data_size      : integer;
        signal data             : in word32;
        constant ram_offset     : integer;
        constant burst_size     : integer;
        signal N_BURSTs_RW0     : out STD_LOGIC_VECTOR ( 31 downto 0 );
        signal BASE_AWADDR0     : out STD_LOGIC_VECTOR ( 31 downto 0 );
        signal M_AXI_WSTRB_TB0  : out STD_LOGIC_VECTOR ( 3 downto 0 );
        signal INIT_AXI_TXN0    : out STD_LOGIC;
        signal WRITE_VALID0     : in STD_LOGIC;
        signal SYSTEM_IDLE0     : in STD_LOGIC;
        signal M_AXI_WDATA_TB0  : out STD_LOGIC_VECTOR ( 31 downto 0 )
        ) is
    variable n_bursts_aux : integer; 
    variable i : integer;     
    variable rest : unsigned(31 downto 0);     

    begin
        M_AXI_WSTRB_TB0 <= (others => '1');
        rest := to_unsigned(data_size, 32) mod to_unsigned(burst_size, 32);

        if rest > 0 then
            n_bursts_aux := data_size / burst_size + clogb2(burst_size) + 2;
        else
            n_bursts_aux := data_size / burst_size + clogb2(burst_size) + 1;
        end if;
        N_BURSTs_RW0 <= std_logic_vector(to_unsigned(n_bursts_aux, 32));
        BASE_AWADDR0 <= std_logic_vector(to_unsigned(wr_addr, 32));
        INIT_AXI_TXN0 <= '1';
        wait until rising_edge(CLK);
        INIT_AXI_TXN0 <= '0';
        i := 0;
        M_AXI_WDATA_TB0 <= data(i + ram_offset);
        loop
            wait until WRITE_VALID0 = '1' and rising_edge(CLK);
            i := i + 1;
            M_AXI_WDATA_TB0 <= data(i + ram_offset);
            if i = data_size - 1 then
                wait until WRITE_VALID0 = '1' and rising_edge(CLK);
                M_AXI_WSTRB_TB0 <= (others => '0');
                exit;
            end if;
        end loop;
        wait until SYSTEM_IDLE0 = '1';

    end procedure;

    procedure recive_data_lite(
        signal data                     : out std_logic_vector ( 31 downto 0 );
        constant addr                   : integer;
        signal m00_axi_init_axi_rxn0    : out std_logic;
        signal m00_axi_rxn_ready0       : in  std_logic;
        signal test_araddr0             : out std_logic_vector ( 31 downto 0 );
        signal test_rdata0               : in std_logic_vector(31 downto 0)
    ) is
    begin

        wait until rising_edge(clk);
        m00_axi_init_axi_rxn0 <= '1';
        test_araddr0 <= std_logic_vector(to_unsigned(addr, 32));
        wait until rising_edge(clk);
        m00_axi_init_axi_rxn0 <= '0';
        wait until m00_axi_rxn_ready0 = '1';
        data <= test_rdata0;
        wait until rising_edge(clk);

    end procedure;
   
    procedure read_data_full(
            constant rd_addr        : integer;
            constant data_size      : integer;
            signal data             : out word32;
            constant ram_offset : integer;
            constant burst_size     : integer;
            signal N_BURSTs_RW0     : out STD_LOGIC_VECTOR ( 31 downto 0 );
            signal BASE_ARADDR0     : out STD_LOGIC_VECTOR ( 31 downto 0 );
            signal INIT_AXI_RXN0    : out STD_LOGIC;
            signal READ_VALID0      : in STD_LOGIC;
            signal M_AXI_RDATA_TB0  : in STD_LOGIC_VECTOR ( 31 downto 0 );
            signal SYSTEM_IDLE0     : in STD_LOGIC
        ) is
        variable n_bursts_aux : integer; 
        variable i : integer;
        variable rest : unsigned(31 downto 0);     
        begin
            rest := to_unsigned(data_size, 32) mod to_unsigned(burst_size, 32);

            if rest > 0 then
                n_bursts_aux := data_size / burst_size + clogb2(burst_size) + 2;
            else
                n_bursts_aux := data_size / burst_size + clogb2(burst_size) + 1;
            end if;
            N_BURSTs_RW0 <= std_logic_vector(to_unsigned(n_bursts_aux, 32));
            BASE_ARADDR0 <= std_logic_vector(to_unsigned(rd_addr, 32));
            INIT_AXI_RXN0 <= '1';
            wait until rising_edge(CLK);
            INIT_AXI_RXN0 <= '0';
            i := 0;
            loop
                if i < data_size then
                    wait until READ_VALID0 = '1' and rising_edge(CLK);
                    data(i + ram_offset) <= M_AXI_RDATA_TB0;
                    i := i + 1;
                else
                    wait until SYSTEM_IDLE0 = '1';
                    exit;
                end if;
            end loop; 
        end procedure;
        
        procedure read_bin_file_word32 (
            constant elements_2_read : integer := 0;
            signal word32_array : out word32;
            constant file_name : in string
        ) is
            type char_file_t is file of character;
            file char_file : char_file_t;
            variable char_v : character;
            subtype byte_t is natural range 0 to 255;
            variable byte_v : byte_t;
            variable index : integer := 0;
            variable n_byte : integer := 0;
            variable aux : std_logic_vector(7 downto 0);
            variable aux32 : std_logic_vector(31 downto 0) := (others => '0');
            variable aux_array_word32  : word32 ;
        begin
            file_open(char_file, file_name);
        
            while not endfile(char_file) and index < elements_2_read loop
                read(char_file, char_v);
                byte_v := character'pos(char_v);
                aux := std_logic_vector(to_unsigned(byte_v, 8));
                aux32 := aux & aux32(31 downto 8);
                n_byte := n_byte + 1;
                if n_byte = 4 then
                    aux_array_word32(index) := aux32;
                    n_byte := 0;
                    index := index + 1;
                end if;
            end loop;
            file_close(char_file);
            word32_array <= aux_array_word32;
        end procedure;


        procedure read_bin_file_word64 (
            constant elements_2_read : integer := 0;
            signal word64_array : out word64;
            constant file_name : in string
        ) is
            type char_file_t is file of character;
            file char_file : char_file_t;
            variable char_v : character;
            subtype byte_t is natural range 0 to 255;
            variable byte_v : byte_t;
            variable index : integer := 0;
            variable n_byte : integer := 0;
            variable aux : std_logic_vector(7 downto 0);
            variable aux64 : std_logic_vector(63 downto 0) := (others => '0');
            variable aux_array_word64  : word64;
        begin
            file_open(char_file, file_name);
        
            while not endfile(char_file) and index < elements_2_read loop
                read(char_file, char_v);
                byte_v := character'pos(char_v);
                aux := std_logic_vector(to_unsigned(byte_v, 8));
                aux64 :=  aux & aux64(63 downto 8);
                n_byte := n_byte + 1;
                if n_byte = 8 then
                    aux_array_word64(index) := aux64;
                    n_byte := 0;
                    index := index + 1;
                end if;
            end loop;
            file_close(char_file);
            word64_array <= aux_array_word64;
        end procedure;
    
        procedure transform_row_2_features (
            constant n_features     : integer;
            constant features_size  : integer := 33;
            signal raw : in word32;
            signal features : out word32;
            signal predictions : out word32
        ) is
            variable rest               : unsigned(31 downto 0);  
            variable index_features     : integer := 0;
            variable index_predictions  : integer := 0;
        begin
            
            for i in 1 to features_size * n_features + 1 loop
                rest := to_unsigned(i, 32) mod to_unsigned(features_size, 32);
                
                if rest = x"00000000" and i > 0 then
                    predictions(index_predictions) <= raw(i - 1);
                    index_predictions := index_predictions + 1;

                else
                    features(index_features) <= raw(i - 1);
                    index_features := index_features + 1;

                end if;

            end loop;

        end procedure;
    
        procedure evaluate_predictions (
            constant n_features        : integer;
            signal actual_prediction   : in word32;
            signal expected_prediction : in word32

        ) is
            variable correct_predictions     : integer := 0;
        begin

        end procedure;


    begin
    clk_proc: process
    begin
        while finished /= '1' loop
            CLK <= '0';
            wait for period_time/2;
            CLK <= '1';
            wait for period_time/2;
        end loop;
        wait;
    end process clk_proc;

    load_data: process
    begin
        data_filled <= '0';
        read_bin_file_word32(n_features * features_size, word32_array, 
                        "C:\Users\ticro\xilinx_proyects\testasd\testasd.srcs\sim_1\new\alzheimers_processed_dataset.bin_tb");

        read_bin_file_word64(n_trees * tree_size, word64_array, 
                        "C:\Users\ticro\xilinx_proyects\testasd\testasd.srcs\sim_1\new\alzheimer.model_tb");

        data_filled <= '1';
        wait;
    end process load_data;
    
    process
    variable correct_predictions     : integer := 0;
    variable error_predictions     : integer := 0;

    begin

        nrst <= '0';
        m00_axi_init_axi_txn <= '0';
        m00_axi_init_axi_rxn <= '0';
        wait until data_filled = '1';
        transform_row_2_features (n_features, features_size, word32_array, word32_features, word32_predictions);
        wait until rising_edge(clk);
        nrst <= '1';
        wait until rising_edge(clk);
        
        -- SEND DATA TREES 32 arboles de 256 elementos
        for i in 0 to n_trees * tree_size - 1 loop
            send_data_lite(to_integer(unsigned(word64_array(i)(31 downto 0))), 
                    TREES_ADDR + 8 * i, m00_axi_init_axi_txn, m00_axi_txn_ready, test_wdata, test_awaddr);

            send_data_lite(to_integer(unsigned(word64_array(i)(63 downto 32))), 
                    TREES_ADDR + 8 * i + 4, m00_axi_init_axi_txn, m00_axi_txn_ready, test_wdata, test_awaddr);
        end loop;

        -- process 64 features de 32 elementos
        send_data_full(FEATURES_ADDR, 32 * n_features, word32_features, 0, 
                        burst_size, N_BURSTs_RW, BASE_AWADDR, M_AXI_WSTRB_TB,
                        INIT_AXI_TXN, WRITE_VALID, SYSTEM_IDLE, M_AXI_WDATA_TB);

        -- -- tell to process 64 featurees
        send_data_lite(max_process_trees, CNTR_N_FEATURES, m00_axi_init_axi_txn, m00_axi_txn_ready, test_wdata, test_awaddr);

        -- -- tell to start
        send_data_lite(16#00000001#, CONTROL_ADDR, m00_axi_init_axi_txn, m00_axi_txn_ready, test_wdata, test_awaddr);
        
        -- wait end
        loop
            recive_data_lite(read_status, CONTROL_ADDR, m00_axi_init_axi_rxn, m00_axi_rxn_ready, test_araddr, test_rdata);
            if read_status(1) = '1' then
                exit;
            end if;
        end loop; --      
        
        -- -- Colect results
        read_data_full(PEDICTION_ADDR, max_process_trees, word32_results, 0, burst_size, N_BURSTs_RW, BASE_ARADDR, 
                    INIT_AXI_RXN, READ_VALID, M_AXI_RDATA_TB, SYSTEM_IDLE);


        for r in 0 to max_process_trees loop
        
            if (to_integer(unsigned(word32_results(r))) <= 0 and word32_predictions(r) = x"00000000") or 
               (to_integer(unsigned(word32_results(r))) > 0 and word32_predictions(r) = one_float)
            then
                correct_predictions := correct_predictions + 1;
            else
                error_predictions := error_predictions + 1;
            end if;
        end loop;
        report "Correct predictions " & integer'image(correct_predictions) & " of "  & integer'image(max_process_trees); 
        report "Error predictions " & integer'image(error_predictions) & " of "  & integer'image(max_process_trees); 
        wait for 10 us;
        finished <= '1';
        
        wait;
    end process;


    u1: design_1_wrapper
    port map
    (
        ------------- debug from tb ----------
        ------------- AXi LITE ---------------
        test_wdata  => test_wdata,
        test_awaddr => test_awaddr,
        test_araddr => test_araddr,
        test_rdata  => test_rdata,
        m00_axi_init_axi_txn => m00_axi_init_axi_txn,
        m00_axi_init_axi_rxn => m00_axi_init_axi_rxn,
        m00_axi_txn_ready => m00_axi_txn_ready,
        m00_axi_rxn_ready => m00_axi_rxn_ready,
        ------------- AXi FULL ---------------
        INIT_AXI_TXN => INIT_AXI_TXN,
        INIT_AXI_RXN => INIT_AXI_RXN,
        BASE_AWADDR => BASE_AWADDR,
        BASE_ARADDR => BASE_ARADDR,
        N_BURSTs_RW => N_BURSTs_RW,
        M_AXI_WDATA_TB => M_AXI_WDATA_TB,
        M_AXI_RDATA_TB => M_AXI_RDATA_TB,
        WRITE_VALID => WRITE_VALID,
        READ_VALID  => READ_VALID,
        M_AXI_WSTRB_TB => M_AXI_WSTRB_TB,
        SYSTEM_IDLE =>  SYSTEM_IDLE,
        --------------------------------------

        
        clk => clk,
        nrst => nrst


    );

end architecture rtl;