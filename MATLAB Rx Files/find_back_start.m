function start = find_back_start(data, preamble)
    length_preamble = length(preamble);
    lag = 1;
    correl = [];
    while(lag <= length(data)-length_preamble+1)
        new_data = data(lag:lag-1+length_preamble);
        corr = (1/length(new_data))*sum((new_data-mean(new_data)).*(preamble-mean(preamble)))/(sqrt(var(new_data)*var(preamble)));
        correl = [correl, corr];
        lag = lag + 100;
    end
    max_corr = max(correl);
    start = find(correl==max_corr);
    start= start-1;
    if(start == [])
        if(data==preamble)
            start = 0;
        end
    end
end