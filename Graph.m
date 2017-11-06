classdef Graph
  methods (Static)
    function main = m()
      f1 = figure;
      M27 = csvread('Report_2_7');
      M29 = csvread('Report_2_9');
      M47 = csvread('Report_4_7');
      M49 = csvread('Report_4_9');
      M67 = csvread('Report_6_7');
      M69 = csvread('Report_6_9');
      plot(M27(:,1), M27(:,3));
      title('Arrivals');
      xlabel('Time Step');
      ylabel('Arrival');
      saveas(f1, 'Arrival', 'png');
      f2 = figure;
      plot(M27(:,1), M27(:,2), 'k', M27(:,1), M29(:,2), 'm', M27(:,1), M47(:,2), 'r', M27(:,1), M49(:,2), 'g', M27(:,1), M67(:,2), 'b', M27(:,1), M69(:,2), 'c');
      title('Waiting Queue');
      xlabel('Time Step');
      ylabel('Waiting');
      ylim([0 1000]);
      legend('N 2 M 7', 'N 2 M 9', 'N 4 M 7', 'N 4 M 9', 'N 6 M 7', 'N 6 M 9', 'Location', 'northeast');
      saveas(f2, 'Waiting', 'png');
      f3 = figure;
      
      subplot(3,1,1);
      plot(M27(:,1), M27(:,4), 'k', M27(:,1), M29(:,4), 'm');
      title('Rejected Customers');
      legend('N 2 M 7', 'N 2 M 9');
      xlabel('Time Step');
      ylabel('Rejected');
      subplot(3,1,2);
      plot(M27(:,1), M47(:,4), 'r', M27(:,1), M49(:,4), 'g');
      legend('N 4 M 7', 'N 4 M 9');
      xlabel('Time Step');
      ylabel('Rejected');
      subplot(3,1,3);
      plot(M27(:,1), M67(:,4), 'b', M27(:,1), M69(:,4), 'c');
      xlabel('Time Step');
      ylabel('Rejected');
      legend('N 6 M 7', 'N 6 M 9', 'Location', 'northeast');
      saveas(f3, 'Rejected', 'png');
     end
  end
end
