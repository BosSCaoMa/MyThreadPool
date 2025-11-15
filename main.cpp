//
// Created by Administrator on 2025/10/12.
//
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
    int numSubarraysWithSum(vector<int>& nums, int goal) {
        int res = 0;
        int n = nums.size();
        
        // 前缀和数组
        vector<int> vecWith(n + 1, 0);  // 这里需要 +1 以便处理从 0 开始的前缀和
        for (int i = 1; i <= n; ++i) {  // 从 1 开始计算前缀和
            vecWith[i] = vecWith[i - 1] + nums[i - 1];  // 计算前缀和
        }

        // 哈希表用于记录前缀和出现的次数
        unordered_map<int, int> um;
        um[0] = 1;  // 初始情况下，前缀和为 0 出现过 1 次

        // 遍历所有前缀和
        for (int i = 1; i <= n; ++i) {  // 从 1 开始遍历前缀和
            int t = vecWith[i];  // 当前的前缀和
            res += um[t - goal];  // 如果 (vecWith[i] - goal) 已出现，说明有符合条件的子数组
            um[t]++;  // 更新前缀和出现的次数
        }

        return res;
    }
};


int main()
{
    Solution s;
    vector<int> nums = {1,0,1,0,1};
    s.numSubarraysWithSum(nums, 2);
}

