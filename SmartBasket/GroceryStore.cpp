///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  #include <iostream>
  #include <string>
  #include <fstream>
  #include <iomanip>
  #include <cstddef>

  #include "GroceryItem.hpp"
  #include "GroceryItemDatabase.hpp"
  #include "GroceryStore.hpp"
/////////////////////// END-TO-DO (1) ////////////////////////////






GroceryStore::GroceryStore( const std::string & persistentInventoryDB )
{
  std::ifstream fin( persistentInventoryDB );                     // Creates the stream object, and then opens the file if it can
                                                                  // The file is closed as fin goes out of scope

  // The file's inventory contents consists of a quoted UPC string followed by a quantity on hand unsigned integer separated by
  // whitespace, like this:
  //     "00044100117428"     8
  //     "00041780001566"    46
  //     "00021000043309"    35
  //     "00021000043309"     9

  ///////////////////////// TO-DO (2) //////////////////////////////
    /// While no errors have been detected and not end-of-file, read the inventory record from the input stream and then add that
    /// information to the memory resident inventory database.
    ///
    /// Hint: Since we didn't define an InventoryRecord class that defines the extraction operator (best practices says we should
    ///       have), extract the quoted string and the quantity attributes directly
    ///
    /// Hint: Just as you did in class GroceryItem, use std::quoted to read quoted strings.  Don't try to parse the quotes yourself.
    ///       See
    ///        1) https://en.cppreference.com/w/cpp/io/manip/quoted
    ///        2) https://www.youtube.com/watch?v=Mu-GUZuU31A
  
  // initialize upc and quantity
  std::string upc;     
  unsigned int quantity = 0;

  while (fin >> std::quoted(upc) >> quantity){ // read file using fin until the end & store each upc and quantity in the inventory database
  _inventoryDB.insert( { upc, quantity } );
    /* _inventoryDB[upc] = quantity; */ // store values in _inventoryDB of the key-value pair, key = upc & value = amount of items on hand (or quantity)
  }

  /////////////////////// END-TO-DO (2) ////////////////////////////
}                                                                 // File is closed as fin goes out of scope







GroceryStore::Inventory_DB & GroceryStore::inventory()
{ return _inventoryDB; }







GroceryStore::GroceryItemsSold GroceryStore::ringUpCustomers( const ShoppingCarts & shoppingCarts, std::ostream & receipt )
{
  GroceryItemsSold todaysSales;                                   // a collection of unique UPCs of grocery items sold

  ///////////////////////// TO-DO (3) //////////////////////////////
    ///  Ring up each customer accumulating the groceries purchased
    ///  Hint:  merge each customer's purchased groceries into today's sales.  (https://en.cppreference.com/w/cpp/container/set/merge)
    
    
    
  for(auto && [name, sc] : shoppingCarts) { // iterate through shopping carts of the customers
    receipt << name << "'s shopping cart contains:\n";         // prints the customer's name using pointer to the key value pair to get the customer name                                    
    
    todaysSales.merge(ringUpCustomer(sc, receipt)); // calculate the item the customer is buying, returns info on the items sold & merge items with todaysSales so the item sold to customer (identified by upc) is linked together with total sales cost as a key-value pair in the map (key value pair: upc, quantity of items)
    
  }


  /////////////////////// END-TO-DO (3) ////////////////////////////

  return todaysSales;
} // ringUpCustomers







GroceryStore::GroceryItemsSold GroceryStore::ringUpCustomer( const ShoppingCart & shoppingCart, std::ostream & receipt )
{
  auto & worldWideGroceryDatabase = GroceryItemDatabase::instance();        // Get a reference to the world wide database of all
                                                                            // groceries in the world. The database will contains a
                                                                            // full description of the item and the item's price.

  GroceryItemsSold purchasedGroceries;                                      // a collection of UPCs of the groceries purchased by this customer


  ///////////////////////// TO-DO (4) //////////////////////////////
    /// Print out a receipt containing a full description of the grocery items (obtained from the database of all groceries in the world) in
    /// the customer's shopping cart along with the total amount due. As items are being scanned, decrement the quantity on hand for
    /// that grocery item in the store's inventory.
    ///
    ///
    /// Hint:  Here's some pseudocode to get you started.
    ///       1        Initialize the amount due to zero
    ///       2        For each grocery item in the customer's shopping cart
    ///       2.1          If the item is not found in the world wide grocery item database, indicate on the receipt it's free of charge
    ///       2.2          Otherwise
    ///       2.2.1            Print the grocery item's full description on the receipt
    ///       2.2.2            Add the grocery item's price to the amount due
    ///       2.2.3            If the grocery item is something the store sells (the item is in the store's inventory)
    ///       2.2.3.1              Decrease the number of items on hand for the item sold
    ///       2.2.3.2              Add the items's UPC to the list of groceries purchased
    ///       3         Print the total amount due on the receipt
    
    double amountDue = 0;  // keeps track of the total cost of the price
   
    for ( auto && [upc, groceryItem] : shoppingCart ) { // iterates through the loop checking each item in worldWideGroceryDatabase and get UPC code for the item (key) and the name of the item (value)
     
      auto dataBaseItem = worldWideGroceryDatabase.find(upc);   // uses find function to find the item in the database based on the item's upc code
      if (dataBaseItem == nullptr) {                                    // if the item is not found print out a message saying so and tell the customer that the item is free of charge
        receipt << "\t" <<  std::quoted(upc) << " (" << groceryItem.productName() << ") not found, the item is free!\n";
      }
      else {                    
        // if the item is found, print out the item's upc, brand name, product name, and price                            
        receipt << "\t" <<  std::quoted(upc) << ", " << std::quoted(dataBaseItem->brandName()) << ", " << std::quoted(dataBaseItem->productName()) << ", " << dataBaseItem->price() <<  "\n";     
        amountDue += dataBaseItem->price();               // add the cost of each item to the total cost of the groceries
      }   
      auto itemUnstock = _inventoryDB.find(upc);
      if (_inventoryDB.find(upc) != _inventoryDB.end())   // verify if the items are in the inventoryDB, if not, add the item by upc and decrement it from the storage
      {
         --itemUnstock->second;  // with each item sold decrement item quantity in the storage; .at() throws exception if item is not found
         purchasedGroceries.insert(upc);                  // add the items in the purchasedGroceries by upc to keep track of the items sold to the customer
        
        
      }
    }
    receipt << "\t" << "-------------------------\n";             // print the total in the txt file format
    receipt <<  "\t" << "Total  $" << amountDue << "\n\n";

    
  /////////////////////// END-TO-DO (4) ////////////////////////////

  return purchasedGroceries;
} // ringUpCustomer







void GroceryStore::reorderItems( GroceryItemsSold & todaysSales, std::ostream & reorderReport )
{
  auto & worldWideGroceryDatabase = GroceryItemDatabase::instance();    // Get a reference to the world wide database of all
                                                                        // groceries in the world. The database will contains a
                                                                        // full description of the item and the item's price.

  ///////////////////////// TO-DO (5) //////////////////////////////
    /// For each grocery item that has fallen below the reorder threshold, assume an order has been placed and now the shipment has
    /// arrived. Update the store's inventory to reflect the additional items on hand.
    ///
    /// Hint:  Here's some pseudocode to get you started.
    ///        1       For each grocery item sold today
    ///        1.1         If the grocery item is not in the store's inventory or if the number of grocery items on hand has fallen below the re-order threshold (REORDER_THRESHOLD)
    ///        1.1.1           If the grocery item is not in the world wide grocery item database,
    ///        1.1.1.1             display just the UPC
    ///        1.1.2           Otherwise,
    ///        1.1.2.1             display the grocery item's full description
    ///        1.1.3           If the grocery item is not in the store's inventory
    ///        1.1.3.1             display a notice indicating the grocery item is no longer sold in this store and will not be re-ordered
    ///        1.1.4           Otherwise,
    ///        1.1.4.1             Display the current quantity on hand and the quantity re-ordered
    ///        1.1.4.2             Increase the quantity on hand by the number of items ordered and received (LOT_COUNT)
    ///        2       Reset the list of grocery item sold today so the list can be reused again later
    ///
    /// Take special care to avoid excessive searches in your solution

    // running low on items in the store, prompt the store to reorder items
    reorderReport << "\n\n\n\nRe-ordering grocery items the store is running low on.\n\n";


    unsigned int updateItem = 0; // update the number of re-ordering iterations made
    for (auto && upc : todaysSales) {    // iterate through the UPC of items sold
        
       // search the map, if grocery item is not found in store inventory & the items in the store are running out below a certain point reorder items to stock up
      if (_inventoryDB.find(upc) == _inventoryDB.end() || _inventoryDB.at(upc) < REORDER_THRESHOLD) {     
        GroceryItem item = *worldWideGroceryDatabase.find(upc);   // reference the grocery item
        if (worldWideGroceryDatabase.find(upc) == nullptr) {      //if item is not found in the worldWideGroceryDatabase 
        
          reorderReport  << "{" << upc << "}\n";                 // just display the item's upc
        }
        else {
          reorderReport << std::right << std::setw(2); // format based on samepleOutput.txt
          // if the grocery item is found in the world wide grocery item database, display the sold Upc items, brand name, and product name
          reorderReport << ++updateItem << ":  {" << std::quoted(upc) << ", " << std::quoted(item.brandName()) << ", " << std::quoted(item.productName()) << "}\n";
        }
        // if the grocery item sold is not found in the inventory, tell the customer the item is not sold in the store and will not be reordered
        if (_inventoryDB.find(upc) == _inventoryDB.end()) {
          reorderReport << "       *** no longer sold in this store and will not be re-ordered" << "\n";  // can not use setw due to asterisks, use spaces instead
        }
        else{
          reorderReport << std::right << std::setw(12);  // format based on samepleOutput.txt
          // if the grocery item sold is found in the inventory, display the current quantity on hand and the quantity re-ordered
          reorderReport << "only " << _inventoryDB.at(upc) << " remain in stock which is " << REORDER_THRESHOLD-_inventoryDB.at(upc);
          reorderReport << " unit(s) below the reorder threshold (" << REORDER_THRESHOLD << "), re-ordering " << LOT_COUNT << " more\n";
          _inventoryDB[upc] += LOT_COUNT;  // updates the quantity after each item is sold from the inventory
        }
      }
    }
    
    todaysSales.clear();  // reset the list of items sold in the grocery list to reuse later
  
    


  /////////////////////// END-TO-DO (5) ////////////////////////////
}







GroceryStore::ShoppingCarts GroceryStore::makeShoppingCarts()
  {
    // Our store has many customers, and each (identified by name) is pushing a shopping cart. Shopping carts are structured as
    // trees of trees of grocery items.
    ShoppingCarts carts =
    {
      // first shopping cart
      { "Red Baron",        { {"00075457129000", {"milk"     }}, {"00038000291210", {"rice krispies"}}, {"00025317533003", {"hotdogs"}},
                              {"09073649000493", {"apple pie"}}, {"00835841005255", {"bread"        }}, {"00688267039317", {"eggs"   }},
                              {"00037466065908", {"Truffles" }}
                            }
      },

      // second shopping cart
      { "Peppermint Patty", { {"00813608012401", {"Cheese"      }}, {"00037466065908", {"Truffles"      }}, {"00631723300704", {"Pepperoncini"}},
                              {"00051500280645", {"Pancake Mix" }}, {"00792851355155", {"Soup"          }}, {"00898425001333", {"Sandwich"    }},
                              {"00021908501871", {"Potato Spuds"}}, {"00856414001092", {"Plantain Chips"}}, {"00036192122930", {"Applesauce"  }},
                              {"00763795931415", {"Paper Plates"}}, {"00070596000647", {"Soap"          }}, {"00079400266200", {"Deodorant"   }}
                            }
      },

      // third shopping cart
      { "Woodstock",        { {"00859013004327", {"Mint Tea"         }}, {"00041331092609", {"Spanish Omelet"}}, {"00217039300005", {"Fillet Steak"   }},
                              {"00636874220338", {"Face Exfoliate"   }}, {"00688267138973", {"Green Tea"     }}, {"00033674100066", {"Forskohlii"     }},
                              {"00891475001421", {"Candy Fruit Chews"}}, {"00688267133442", {"Coffee"        }}, {"00021000778690", {"Cheesy Potatoes"}},
                              {"00071758100823", {"Pepper"           }}, {"00041520010667", {"Mouth Wash"    }}, {"00216079600007", {"Lamb Chops"     }},
                              {"00790555051526", {"Black Beans"      }}, {"00070177862640", {"Tea Sampler"   }}, {"00018000001958", {"Cinnamon Rolls" }}
                            }
      },

      // forth shopping cart
      { "Schroeder",        { {"00072591002251", {"Peanut Butter"}}, {"00219019100001", {"Ground Beef"           }}, {"00890692002020", {"Stretch Cream" }},
                              {"00637876981630", {"Mozzarella"   }}, {"00716237183507", {"Facial Towelettes"     }}, {"00886911000052", {"Pastry Popover"}},
                              {"00084783499008", {"Massage Oil"  }}, {"00016055456747", {"Children's Fruit Drink"}}, {"00021908743325", {"Granola Cereal"}},
                              {"00688267138973", {"Green Tea"    }}
                            }
      },

      // fifth shopping cart
      { "Lucy van Pelt",    { {"00070327012116", {"Pineapple"     }}, {"00073377801839", {"Sleep Mask Blue"}}, {"00030768009670", {"Calcium Softgels"}},
                              {"00852697001422", {"Baby Food"     }}, {"00079471102414", {"Manzanillo"     }}, {"00072736013753", {"Dressing"        }},
                              {"00216210900003", {"Cheddar Cheese"}}, {"00780872310185", {"Dog Food"       }}, {"00710069086036", {"Crackers"        }},
                              {"00092657016985", {"Tea",          }}, {"00086449903065", {"Deodorant"      }}, {"00885229000198", {"Drinks"          }}
                            }
      },

      // sixth shopping cart
      { "Charlie Brown",    { {"00611508524006", {"Lump Of Coal"}}
                            }
      }
    };

    return carts;
  }  // makeShoppingCarts
