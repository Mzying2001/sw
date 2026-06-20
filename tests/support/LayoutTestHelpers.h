#pragma once

#include "ILayout.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace swtest
{
    namespace layouttest
    {
        /**
         * @brief 记录测量和排列调用的轻量布局假对象。
         *
         * 该类型用于在布局单元测试中替代真实UIElement。Measure会记录传入的可用尺寸，
         * 并将DesiredSize更新为预设结果；Arrange会记录最终排列矩形。若提供log指针，
         * 每次调用还会按发生顺序写入一条文本记录，便于测试布局算法的调用顺序。
         */
        class RecordingLayout : public sw::ILayout
        {
        public:
            /**
             * @brief 用于调用日志中的元素名称
             */
            std::string name;

            /**
             * @brief 通过GetLayoutTag返回的布局标记
             */
            uint64_t layoutTag = 0;

            /**
             * @brief 每次Measure后写入desiredSize的预设测量结果
             */
            sw::Size measureResultSize{};

            /**
             * @brief 通过GetDesireSize返回的当前期望尺寸
             */
            sw::Size desiredSize{};

            /**
             * @brief Measure被调用的次数
             */
            int measureCount = 0;

            /**
             * @brief Arrange被调用的次数
             */
            int arrangeCount = 0;

            /**
             * @brief 最近一次Measure收到的可用尺寸
             */
            sw::Size lastMeasureAvailableSize{};

            /**
             * @brief 最近一次Arrange收到的最终矩形
             */
            sw::Rect lastArrangeRect{};

            /**
             * @brief 全部Measure调用的可用尺寸记录
             */
            std::vector<sw::Size> measureAvailableSizes{};

            /**
             * @brief 全部Arrange调用的最终矩形记录
             */
            std::vector<sw::Rect> arrangeRects{};

            /**
             * @brief 可选的共享调用日志，记录格式为“name.Measure”或“name.Arrange”
             */
            std::vector<std::string> *log = nullptr;

            /**
             * @brief 创建一个使用默认字段值的记录布局对象
             */
            RecordingLayout() = default;

            /**
             * @brief 创建一个带名称、测量结果和布局标记的记录布局对象
             * @param name 用于调用日志中的元素名称
             * @param measureResultSize Measure后写入desiredSize的预设测量结果
             * @param layoutTag 通过GetLayoutTag返回的布局标记
             * @param log 可选的共享调用日志
             */
            RecordingLayout(
                std::string name,
                sw::Size measureResultSize,
                uint64_t layoutTag = 0,
                std::vector<std::string> *log = nullptr)
                : name(std::move(name)),
                  layoutTag(layoutTag),
                  measureResultSize(measureResultSize),
                  desiredSize(measureResultSize),
                  log(log)
            {
            }

            /**
             * @brief 获取布局标记
             */
            virtual uint64_t GetLayoutTag() const override
            {
                return this->layoutTag;
            }

            /**
             * @brief 获取当前期望尺寸
             */
            virtual sw::Size GetDesireSize() const override
            {
                return this->desiredSize;
            }

            /**
             * @brief 获取子布局数量，RecordingLayout始终为叶子节点
             */
            virtual int GetChildLayoutCount() const override
            {
                return 0;
            }

            /**
             * @brief 获取子布局，RecordingLayout没有子节点，调用时总是抛出异常
             * @throw std::out_of_range 始终抛出
             */
            virtual sw::ILayout &GetChildLayoutAt(int) const override
            {
                throw std::out_of_range("RecordingLayout has no children.");
            }

            /**
             * @brief 记录测量调用并更新期望尺寸
             * @param availableSize 布局算法传入的可用尺寸
             */
            virtual void Measure(const sw::Size &availableSize) override
            {
                this->lastMeasureAvailableSize = availableSize;
                this->measureAvailableSizes.push_back(availableSize);
                ++this->measureCount;
                this->desiredSize = this->measureResultSize;

                if (this->log != nullptr) {
                    this->log->push_back(this->name + ".Measure");
                }
            }

            /**
             * @brief 记录排列调用
             * @param finalPosition 布局算法传入的最终矩形
             */
            virtual void Arrange(const sw::Rect &finalPosition) override
            {
                this->lastArrangeRect = finalPosition;
                this->arrangeRects.push_back(finalPosition);
                ++this->arrangeCount;

                if (this->log != nullptr) {
                    this->log->push_back(this->name + ".Arrange");
                }
            }
        };

        /**
         * @brief 为LayoutHost提供子元素列表的纯内存容器。
         *
         * 该类型同样实现ILayout，但主要用于被LayoutHost::Associate绑定。测试可通过
         * EmplaceChild创建稳定地址的RecordingLayout子节点，然后让被测布局对象通过
         * GetChildLayoutCount和GetChildLayoutAt访问这些节点。
         */
        class ContainerLayout : public sw::ILayout
        {
        private:
            /**
             * @brief 持有子布局对象，保证其地址在测试生命周期内稳定
             */
            std::vector<std::unique_ptr<RecordingLayout>> _children{};

        public:
            /**
             * @brief 传递给新建子节点的共享调用日志
             */
            std::vector<std::string> *log = nullptr;

            /**
             * @brief 通过GetLayoutTag返回的容器布局标记
             */
            uint64_t layoutTag = 0;

            /**
             * @brief 通过GetDesireSize返回的容器期望尺寸
             */
            sw::Size desiredSize{};

            /**
             * @brief 创建一个纯内存布局容器
             * @param log 可选的共享调用日志，创建子节点时会传递给RecordingLayout
             */
            explicit ContainerLayout(std::vector<std::string> *log = nullptr)
                : log(log)
            {
            }

            /**
             * @brief 在容器末尾创建一个RecordingLayout子节点
             * @param name 用于子节点调用日志中的元素名称
             * @param measureResultSize 子节点Measure后的预设期望尺寸
             * @param layoutTag 子节点通过GetLayoutTag返回的布局标记
             * @return 新创建子节点的引用
             */
            RecordingLayout &EmplaceChild(
                std::string name,
                sw::Size measureResultSize,
                uint64_t layoutTag = 0)
            {
                std::unique_ptr<RecordingLayout> child(
                    new RecordingLayout(std::move(name), measureResultSize, layoutTag, this->log));
                this->_children.emplace_back(std::move(child));
                return *this->_children.back();
            }

            /**
             * @brief 获取容器自身的布局标记
             */
            virtual uint64_t GetLayoutTag() const override
            {
                return this->layoutTag;
            }

            /**
             * @brief 获取容器自身的期望尺寸
             */
            virtual sw::Size GetDesireSize() const override
            {
                return this->desiredSize;
            }

            /**
             * @brief 获取子布局数量
             */
            virtual int GetChildLayoutCount() const override
            {
                return static_cast<int>(this->_children.size());
            }

            /**
             * @brief 获取指定索引处的子布局
             * @throw std::out_of_range 索引越界时抛出
             */
            virtual sw::ILayout &GetChildLayoutAt(int index) const override
            {
                return *this->_children.at(static_cast<size_t>(index));
            }

            /**
             * @brief 容器自身的测量操作为空实现
             */
            virtual void Measure(const sw::Size &) override
            {
            }

            /**
             * @brief 容器自身的排列操作为空实现
             */
            virtual void Arrange(const sw::Rect &) override
            {
            }
        };
    }
}
