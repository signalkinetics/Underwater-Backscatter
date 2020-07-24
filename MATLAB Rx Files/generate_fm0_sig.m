function p = generate_fm0_sig(tx_code, fm0_samps)
    clk = 1;
    state_AB = [1, 1];
    p = [];
    for k=1:length(tx_code)
       for m = 1:2
          if clk == 1
             state_AB = [(~state_AB(2)) & 1, xor(tx_code(k), (state_AB(2) & 1)) & 1];
          end
          gen_sig = ((clk&1) * state_AB(1) + ((~clk)&1)*state_AB(2)) & 1;
          % make DC center at 0
          gen_sig = (gen_sig - 0.5);
          if (m == 1) % deal with odd size sample
              p = [p, gen_sig*ones(1, floor(fm0_samps/2))];
          else
              p = [p, gen_sig*ones(1, ceil(fm0_samps/2))]; 
          end
          clk = ~clk;
       end
       
    end
end