#include "Test.h"

#include "ListView.h"

#include <algorithm>
#include <commctrl.h>
#include <string>
#include <vector>

namespace
{
    class TestListView : public sw::ListView
    {
    public:
        bool NotifyHeaderEndDrag(int index, int order, bool includeOrder = true)
        {
            HDITEMW item{};
            item.mask   = includeOrder ? HDI_ORDER : 0;
            item.iOrder = order;

            NMHEADERW notification{};
            notification.hdr.hwndFrom = ListView_GetHeader(Handle);
            notification.hdr.code     = HDN_ENDDRAG;
            notification.iItem        = index;
            notification.pitem        = &item;

            LRESULT result = 0;
            return OnNotify(&notification.hdr, result);
        }

        RECT GetCheckBoxRectForTest(int index)
        {
            RECT rect{};
            GetCheckBoxRect(index, rect);
            return rect;
        }
    };

    void InitializeListViewControls()
    {
        static bool initialized = []() {
            INITCOMMONCONTROLSEX controls{};
            controls.dwSize = sizeof(controls);
            controls.dwICC  = ICC_LISTVIEW_CLASSES;
            return InitCommonControlsEx(&controls) != FALSE;
        }();

        REQUIRE(initialized);
    }

    void AddColumns(TestListView &listView, int count)
    {
        for (int i = 0; i < count; ++i) {
            listView.Columns->Add(
                sw::ListViewColumn(std::wstring(L"Column") + std::to_wstring(i)));
        }
    }

    std::vector<int> GetNativeColumnOrder(const TestListView &listView)
    {
        int count = listView.Columns->Count();
        std::vector<int> order(static_cast<size_t>(count), -1);

        if (count > 0) {
            REQUIRE(ListView_GetColumnOrderArray(
                listView.Handle, count, order.data()));
        }
        return order;
    }

    std::vector<int> GetModelColumnOrder(const TestListView &listView)
    {
        int count = listView.Columns->Count();
        std::vector<int> order(static_cast<size_t>(count), -1);

        for (int index = 0; index < count; ++index) {
            int displayOrder = listView.Columns->GetAt(index).order;
            if (displayOrder >= 0 && displayOrder < count) {
                order[static_cast<size_t>(displayOrder)] = index;
            }
        }
        return order;
    }

    void CheckColumnOrder(
        const TestListView &listView, const std::vector<int> &expected)
    {
        auto nativeOrder = GetNativeColumnOrder(listView);
        auto modelOrder  = GetModelColumnOrder(listView);

        REQUIRE_EQ(expected.size(), nativeOrder.size());
        REQUIRE_EQ(expected.size(), modelOrder.size());

        for (size_t i = 0; i < expected.size(); ++i) {
            CHECK_EQ(expected[i], nativeOrder[i]);
            CHECK_EQ(expected[i], modelOrder[i]);
        }
    }

    void CheckModelColumnOrder(
        const TestListView &listView, const std::vector<int> &expected)
    {
        auto modelOrder = GetModelColumnOrder(listView);

        REQUIRE_EQ(expected.size(), modelOrder.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            CHECK_EQ(expected[i], modelOrder[i]);
        }
    }

    void CheckModelMatchesNative(const TestListView &listView)
    {
        auto nativeOrder = GetNativeColumnOrder(listView);
        auto modelOrder  = GetModelColumnOrder(listView);

        REQUIRE_EQ(nativeOrder.size(), modelOrder.size());
        for (size_t i = 0; i < nativeOrder.size(); ++i) {
            CHECK_EQ(nativeOrder[i], modelOrder[i]);
        }

        auto sorted = modelOrder;
        std::sort(sorted.begin(), sorted.end());
        for (size_t i = 0; i < sorted.size(); ++i) {
            CHECK_EQ(static_cast<int>(i), sorted[i]);
        }
    }

    std::wstring GetNativeColumnHeader(
        const TestListView &listView, int index)
    {
        wchar_t buffer[128]{};
        LVCOLUMNW column{};
        column.mask       = LVCF_TEXT;
        column.pszText    = buffer;
        column.cchTextMax = static_cast<int>(sizeof(buffer) / sizeof(buffer[0]));

        REQUIRE(ListView_GetColumn(listView.Handle, index, &column));
        return buffer;
    }
}

TEST_CASE("ListView columns synchronize default and programmatic display order")
{
    InitializeListViewControls();

    sw::ListViewColumn defaultColumn;
    CHECK_EQ(100.0, defaultColumn.width);
    CHECK_EQ(-1, defaultColumn.order);
    CHECK_EQ(-1, defaultColumn.imageIndex);
    CHECK(defaultColumn.alignment == sw::ListViewColumnAlignment::Left);

    TestListView listView;
    REQUIRE(listView.Handle != NULL);
    CHECK(listView.SendMessageW(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0) &
          LVS_EX_HEADERDRAGDROP);

    AddColumns(listView, 4);
    CheckColumnOrder(listView, {0, 1, 2, 3});

    auto column = listView.Columns->GetAt(0);
    column.order = 3;
    listView.Columns->SetAt(0, column);
    CheckColumnOrder(listView, {1, 2, 3, 0});

    column       = listView.Columns->GetAt(2);
    column.order = 0;
    listView.Columns->SetAt(2, column);
    CheckColumnOrder(listView, {2, 1, 3, 0});

    column       = listView.Columns->GetAt(1);
    column.order = 99;
    listView.Columns->SetAt(1, column);
    CheckColumnOrder(listView, {2, 1, 3, 0});
}

TEST_CASE("ListView predicts header drag order before the native header commits it")
{
    InitializeListViewControls();

    TestListView listView;
    AddColumns(listView, 3);

    CHECK_FALSE(listView.NotifyHeaderEndDrag(0, 2));
    CheckModelColumnOrder(listView, {1, 2, 0});

    std::vector<int> nativeOrder{1, 2, 0};
    REQUIRE(ListView_SetColumnOrderArray(
        listView.Handle, static_cast<int>(nativeOrder.size()), nativeOrder.data()));
    CheckColumnOrder(listView, nativeOrder);

    CHECK_FALSE(listView.NotifyHeaderEndDrag(0, 0));
    CheckModelColumnOrder(listView, {0, 1, 2});

    nativeOrder = {0, 1, 2};
    REQUIRE(ListView_SetColumnOrderArray(
        listView.Handle, static_cast<int>(nativeOrder.size()), nativeOrder.data()));
    CheckColumnOrder(listView, nativeOrder);

    CHECK_FALSE(listView.NotifyHeaderEndDrag(1, 1));
    CheckModelColumnOrder(listView, nativeOrder);

    CHECK_FALSE(listView.NotifyHeaderEndDrag(-1, 0));
    CHECK_FALSE(listView.NotifyHeaderEndDrag(1, 3));
    CHECK_FALSE(listView.NotifyHeaderEndDrag(1, 0, false));
    CheckModelColumnOrder(listView, nativeOrder);

    CHECK_EQ(std::wstring(L"Column0"), listView.Columns->GetAt(0).header);
    CHECK_EQ(std::wstring(L"Column1"), listView.Columns->GetAt(1).header);
    CHECK_EQ(std::wstring(L"Column2"), listView.Columns->GetAt(2).header);
}

TEST_CASE("ListView refresh and collection changes preserve valid column orders")
{
    InitializeListViewControls();

    TestListView listView;
    AddColumns(listView, 4);

    const int desiredOrders[] = {2, 3, 1, 0};
    for (int index = 0; index < 4; ++index) {
        listView.Columns->GetAt(index).order = desiredOrders[index];
    }

    listView.Refresh();
    CheckColumnOrder(listView, {3, 2, 0, 1});

    listView.Columns->Move(0, 3);
    CheckColumnOrder(listView, {2, 1, 3, 0});

    for (int index = 0; index < listView.Columns->Count(); ++index) {
        CHECK_EQ(
            listView.Columns->GetAt(index).header,
            GetNativeColumnHeader(listView, index));
    }

    sw::ListViewColumn replacement(L"Replacement");
    replacement.order = 0;
    listView.Columns->SetAt(1, replacement);
    CheckColumnOrder(listView, {1, 2, 3, 0});
    CHECK_EQ(std::wstring(L"Replacement"), GetNativeColumnHeader(listView, 1));

    listView.Columns->Insert(2, sw::ListViewColumn(L"Inserted"));
    CheckModelMatchesNative(listView);

    listView.Columns->RemoveAt(3);
    CheckModelMatchesNative(listView);

    auto nativeOrder = GetNativeColumnOrder(listView);
    listView.Columns->GetAt(0).order = 0;
    listView.Columns->GetAt(1).order = 0;
    listView.Refresh();
    CheckColumnOrder(listView, nativeOrder);

    listView.Columns->Clear();
    CHECK_EQ(0, Header_GetItemCount(ListView_GetHeader(listView.Handle)));

    AddColumns(listView, 2);
    CheckColumnOrder(listView, {0, 1});
}

TEST_CASE("ListView checkbox rectangle follows logical column zero after reordering")
{
    InitializeListViewControls();

    TestListView listView;
    listView.Rect       = sw::Rect(0, 0, 400, 200);
    listView.CheckBoxes = true;

    AddColumns(listView, 3);
    listView.Items->Add(sw::ListViewItem({L"First", L"Second", L"Third"}));

    auto firstColumn = listView.Columns->GetAt(0);
    firstColumn.order = 2;
    listView.Columns->SetAt(0, firstColumn);
    CheckColumnOrder(listView, {1, 2, 0});

    RECT checkRect = listView.GetCheckBoxRectForTest(0);

    HWND header = ListView_GetHeader(listView.Handle);
    RECT columnRect{};
    REQUIRE(Header_GetItemRect(header, 0, &columnRect));
    MapWindowPoints(
        header, listView.Handle, reinterpret_cast<POINT *>(&columnRect), 2);

    CHECK_GT(columnRect.left, 0);
    CHECK_GT(checkRect.right, checkRect.left);
    CHECK_GT(checkRect.bottom, checkRect.top);
    CHECK_GE(checkRect.left, columnRect.left);
    CHECK_LE(checkRect.right, columnRect.right);

    CHECK_EQ(std::wstring(L"Column0"), listView.Columns->GetAt(0).header);
    CHECK_EQ(std::wstring(L"First"), listView.Items->GetAt(0).subItems[0]);
    CHECK_EQ(std::wstring(L"Second"), listView.Items->GetAt(0).subItems[1]);
    CHECK_EQ(std::wstring(L"Third"), listView.Items->GetAt(0).subItems[2]);
}
