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
         * @brief 记录测量和排列调用的轻量布局假对象
         */
        class RecordingLayout : public sw::ILayout
        {
        public:
            std::string name;
            uint64_t layoutTag = 0;
            sw::Size measureResultSize{};
            sw::Size desiredSize{};
            int measureCount = 0;
            int arrangeCount = 0;
            sw::Size lastMeasureAvailableSize{};
            sw::Rect lastArrangeRect{};
            std::vector<sw::Size> measureAvailableSizes{};
            std::vector<sw::Rect> arrangeRects{};
            std::vector<std::string> *log = nullptr;

            RecordingLayout() = default;

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

            virtual uint64_t GetLayoutTag() const override
            {
                return this->layoutTag;
            }

            virtual sw::Size GetDesireSize() const override
            {
                return this->desiredSize;
            }

            virtual int GetChildLayoutCount() const override
            {
                return 0;
            }

            virtual sw::ILayout &GetChildLayoutAt(int) const override
            {
                throw std::out_of_range("RecordingLayout has no children.");
            }

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
         * @brief 为布局宿主提供子元素列表的纯内存容器
         */
        class ContainerLayout : public sw::ILayout
        {
        private:
            std::vector<std::unique_ptr<RecordingLayout>> _children{};

        public:
            std::vector<std::string> *log = nullptr;
            uint64_t layoutTag = 0;
            sw::Size desiredSize{};

            explicit ContainerLayout(std::vector<std::string> *log = nullptr)
                : log(log)
            {
            }

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

            virtual uint64_t GetLayoutTag() const override
            {
                return this->layoutTag;
            }

            virtual sw::Size GetDesireSize() const override
            {
                return this->desiredSize;
            }

            virtual int GetChildLayoutCount() const override
            {
                return static_cast<int>(this->_children.size());
            }

            virtual sw::ILayout &GetChildLayoutAt(int index) const override
            {
                return *this->_children.at(static_cast<size_t>(index));
            }

            virtual void Measure(const sw::Size &) override
            {
            }

            virtual void Arrange(const sw::Rect &) override
            {
            }
        };
    }
}
