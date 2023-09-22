exports = function(payload){
const mongodb = context.services.get("mongodb-atlas");
const itemsCollection = mongodb.db("ASSET_INFO").collection("New_Luggage_data");
const query = { "Tag_id": payload.body.text()};

itemsCollection.deleteOne(query);

   /*For Updating lock status*/     
    const mongodb2 = context.services.get("mongodb-atlas");
    const itemsCollection2 = mongodb2.db("ASSET_INFO").collection("Lock_Status");
    
    const query2 = { "Tag_id": payload.body.text()};
    const update2 = {
      "$set": {
        "Lock_Status": "0"
      }
    };
    const options2 = { "upsert": false };// upsert  means update or (insert if field unavaible)
    
    itemsCollection2.updateOne(query2, update2, options2)

};