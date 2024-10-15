void pile_up() {
    // 定义测试时间，单位：毫秒
    const int all = 2000000;
    // 堆积事件数量
    int pile_up = 0;
    // 记录触发事件数量
    int Event_all = 0;

    // 泊松分布参数
    double event_ratio = 60; // 60 Hz
    // 将泊松分布参数从秒转换为毫秒
    double lamda = event_ratio * 0.001;

    // 记录每个时间点的事件发生情况
    int event_flag[all] = {0};
    // 记录每个时间点探测器的工作状态
    int work_flag[all] = {0};

    TRandom3 randGen(0);

    for (int i = 0; i < all; ++i) {
        int temp = randGen.Poisson(lamda);
        if (temp) {
            Event_all++;
            event_flag[i] = temp;
            // 如果探测器不在死时间内，则更改接下来 3 ms 的工作状态
            if (!work_flag[i]) {
                work_flag[i] = 1;
                if (i + 1 < all) work_flag[i + 1] = 1;
                if (i + 2 < all) work_flag[i + 2] = 1;
            }
            else pile_up++; // 如果在死时间内发生事件，计入堆积事件数
        }
    }

    // 输出结果
    cout << "Event All: " << Event_all << "; Pile-up Event: " << pile_up << "; Time All: " << all << " ms" << endl;
    // 计算事件堆积率
    double pile_up_rate = pile_up / static_cast<double>(Event_all);
    cout << "Pile-up Rate: " << pile_up_rate << endl;
    // 检查是否达到了预设的事件率
    cout << "Event Ratio: " << Event_all / static_cast<double>(all) * 1000 << " Hz" << endl;
}