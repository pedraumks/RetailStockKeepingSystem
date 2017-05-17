//  File: stocksystem.cpp
//  Author: Pedraum Safarzadeh
//  Date: 2016-06-11
//  Description: Implementation of a StockSystem class

#include <stdio.h>
#include <iostream>
#include <stdexcept>

#include "stocksystem.h"

using namespace std;

// default constructor;
// begin with a balance of $100,000.00
StockSystem::StockSystem() {
    balance = 100000.00;
}

// returns the balance member
double StockSystem::GetBalance() {
    return balance;
}

// Add a new SKU to the system. Do not allow insertion of duplicate sku
bool StockSystem::StockNewItem(StockItem item) {
    if (records.Insert(item))
        return true;
    else
        return false;
}

// Locate the item with key itemsku and update its description field.
// Return false if itemsku is not found.
bool StockSystem::EditStockItemDescription(int itemsku, string desc) {
    StockItem* item = records.Retrieve(StockItem(itemsku, " ", 0));
    if (item == NULL)
        return false;
    else {
        item->SetDescription(desc);
        return true;
    }
}

// Locate the item with key itemsku and update its description field.
// Return false if itemsku is not found or retailprice is negative.
bool StockSystem::EditStockItemPrice(int itemsku, double retailprice) {
    StockItem* item = records.Retrieve(StockItem(itemsku, " ", 0));
    if (item == NULL || retailprice < 0)
        return false;
    else {
        item->SetPrice(retailprice);
        return true;
    }
}

// Purchase quantity of item at unitprice each, to reach a maximum (post-purchase) on-hand stock quantity of 1000.
// Return false if balance is not sufficient to make the purchase,
//   or if SKU does not exist, or if quantity or unitprice are negative.
// Otherwise, return true and increase the item's on-hand stock by quantity,
//   and reduce balance by quantity*unitprice.
bool StockSystem::Restock(int itemsku, int quantity, double unitprice) {
    StockItem* item = records.Retrieve(StockItem(itemsku, " ", 0));
    double reduce_factor = quantity * unitprice;
    if (item == NULL || quantity < 0 || unitprice < 0 || balance - reduce_factor < 0)
        return false;
    else {
        if ((quantity + item->GetStock()) < 1000) {
            item->SetStock(item->GetStock() + quantity);
            balance -= reduce_factor;
            return true;
        }
        else {
            balance -= (1000 - item->GetStock()) * unitprice;
            item->SetStock(1000);
            return true;
        }
    }
}

// Sell an item to a customer, if quantity of stock is available and SKU exists.
// Reduce stock by quantity, increase balance by quantity*price, and return true if stock available.
// If partial stock (less than quantity) available, sell the available stock and return true.
// If no stock, sku does not exist, or quantity is negative, return false.
bool StockSystem::Sell(int itemsku, int quantity) {
    StockItem* item = records.Retrieve(StockItem(itemsku, " ", 0));
    double increase_factor = quantity * item->GetPrice();
    if (item == NULL || quantity < 0 || item->GetStock() <= 0)
        return false;
    else {
        if (quantity > item->GetStock()) {
            balance += item->GetStock() * item->GetPrice();
            item->SetStock(0);
            return true;
        }
        else {
            balance += increase_factor;
            item->SetStock(item->GetStock() - quantity);
            return true;
        }
    }
}
