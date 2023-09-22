exports = function() {
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("New_Luggage_data");
//  const purchasesCollection = mongodb.db("store").collection("purchases");
  // ... paste snippet here ...

  
const query = { "name": "saurav" };
const update = {
  "$set": {
    "name": "saurav_updated",
    "price": 20.99,
    "category": "toys"
  }
};
const options = { "upsert": false };

itemsCollection.updateOne(query, update, options)
/*  .then(result => {
    const { matchedCount, modifiedCount } = result;
    if(matchedCount && modifiedCount) {
      console.log(`Successfully updated the item.`)
    }
  })
  .catch(err => console.error(`Failed to update the item: ${err}`))
  */
}