for i = 1:6
    ExpA_norm(i,:) = ExpA(i,:)./ExpA(i,1);
end

Sum_A = sum(ExpA)./6;

for i = 1:6
    gen_sum(i) = sum(Gen(:,i))/sum(Gen(:,i)>0);
end

for i = 1:3
    diff(i) = sum(ExpB_diff(:,i))/sum(ExpB_diff(:,i)>0);
    dist(i) = sum(ExpB_dist(:,i))/sum(ExpB_dist(:,i)>0);
    eff(i) = sum(ExpB_eff(:,i))/sum(ExpB_eff(:,i)>0);
    eng(i) = sum(ExpB_eng(:,i))/sum(ExpB_eng(:,i)>0);
end

plot(diff)

figure 
bar([0.2,0.3,0.4,0.5,0.6],Sum_A(1:5))
ylabel('Subjective Difficulty')
xlabel('Assistance (N/mm)')
xticks([0.2,0.3,0.4,0.5,0.6])



figure 
bar([0.2,0.3,0.4,0.5,0.6],Sum_A(6:10))
ylabel('Subjective Difficulty')
xlabel('Resistance (N/mm^2)')
xticks([0.2,0.3,0.4,0.5,0.6])