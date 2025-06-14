#pragma once

#include "SimpleWindow.h"
#include <vector>

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
    // 网格面板
    sw::Grid grid;

    // 编辑框
    sw::TextBox tb;

    // 储存按钮的vector
    std::vector<std::unique_ptr<sw::Button>> btns;

    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent()
    {
        // 设置窗口布局方式
        SetLayout<sw::FillLayout>();

        /**
         * Grid是由列和行组成的灵活的网格区域
         * 使用SetRows和SetColumns函数可以设置网格的行列信息
         * 通过修改控件的LayoutTag即可设置控件所在的行列以及跨行列数
         *
         * Grid的行列具有以下几种类型：
         *  - FixSize    固定大小，即宽度或者高度为固定值
         *  - AutoSize   自动大小，此类行或列会根据所在该行或列的控件自动调整大小
         *  - FillRemain 填充剩余空间，此类行或列会填充除上面两种类型行或列之外的剩余控件，有多个FillRemain类型的行或列时支持按照权重分割剩余空间（默认权重为1）
         *
         * GridLayoutTag有以下字段：
         *  - row        指定控件所在的行
         *  - column     指定控件所在的列
         *  - rowSpan    指定控件所跨行数
         *  - columnSpan 指定控件所跨列数
         */

        // 设置网格面板的行信息
        grid.SetRows({
            sw::FillRemainGridRow{},
            sw::AutoSizeGridRow{},
            sw::AutoSizeGridRow{},
            sw::AutoSizeGridRow{},
            sw::AutoSizeGridRow{},
            sw::AutoSizeGridRow{},
            sw::AutoSizeGridRow{},
            sw::FillRemainGridRow{},
        });

        // 设置网格面板的列信息
        grid.SetColumns({
            sw::FillRemainGridColumn{},
            sw::AutoSizeGridColumn{},
            sw::AutoSizeGridColumn{},
            sw::AutoSizeGridColumn{},
            sw::AutoSizeGridColumn{},
            sw::FillRemainGridColumn{},
        });

        // 添加网格面板到界面
        AddChild(grid);

        // 初始化编辑框
        tb.HorizontalAlignment = sw::HorizontalAlignment::Stretch;

        // 添加编辑框到界面
        grid.AddChild(tb, sw::GridLayoutTag{1, 1, 1, 4}); // （序列从0开始）第一行、第一列、跨一行四列

        // 创建按钮并设置对齐方式
        for (int i = 0; i < 17; ++i)
        {
            btns.emplace_back(new sw::Button); // 创建对象并加入vector
            btns.back()->SetAlignment(sw::HorizontalAlignment::Stretch, sw::VerticalAlignment::Stretch);
        }

        // 设置按钮标题
        btns[0]->Text = L"0";
        btns[1]->Text = L"1";
        btns[2]->Text = L"2";
        btns[3]->Text = L"3";
        btns[4]->Text = L"4";
        btns[5]->Text = L"5";
        btns[6]->Text = L"6";
        btns[7]->Text = L"7";
        btns[8]->Text = L"8";
        btns[9]->Text = L"9";
        btns[10]->Text = L"+";
        btns[11]->Text = L"-";
        btns[12]->Text = L"*";
        btns[13]->Text = L"/";
        btns[14]->Text = L".";
        btns[15]->Text = L"=";
        btns[16]->Text = L"Del";

        // 添加按钮到界面
        grid.AddChild(btns[0].get(), sw::GridLayoutTag{5, 1, 1, 2});  // 数字0键
        grid.AddChild(btns[1].get(), sw::GridLayoutTag{2, 1});        // 数字1键
        grid.AddChild(btns[2].get(), sw::GridLayoutTag{2, 2});        // 数字2键
        grid.AddChild(btns[3].get(), sw::GridLayoutTag{2, 3});        // 数字3键
        grid.AddChild(btns[4].get(), sw::GridLayoutTag{3, 1});        // 数字4键
        grid.AddChild(btns[5].get(), sw::GridLayoutTag{3, 2});        // 数字5键
        grid.AddChild(btns[6].get(), sw::GridLayoutTag{3, 3});        // 数字6键
        grid.AddChild(btns[7].get(), sw::GridLayoutTag{4, 1});        // 数字7键
        grid.AddChild(btns[8].get(), sw::GridLayoutTag{4, 2});        // 数字8键
        grid.AddChild(btns[9].get(), sw::GridLayoutTag{4, 3});        // 数字9键
        grid.AddChild(btns[10].get(), sw::GridLayoutTag{2, 4});       // 加号键
        grid.AddChild(btns[11].get(), sw::GridLayoutTag{3, 4});       // 减号键
        grid.AddChild(btns[12].get(), sw::GridLayoutTag{4, 4});       // 乘号键
        grid.AddChild(btns[13].get(), sw::GridLayoutTag{5, 4});       // 除号键
        grid.AddChild(btns[14].get(), sw::GridLayoutTag{5, 3});       // 小数点键
        grid.AddChild(btns[15].get(), sw::GridLayoutTag{6, 3, 1, 2}); // 等号键
        grid.AddChild(btns[16].get(), sw::GridLayoutTag{6, 1, 1, 2}); // 删除键
    }

public:
    /**
     * @brief MyWindow构造函数
     */
    MyWindow()
    {
        InitializeComponent();
    }
};
