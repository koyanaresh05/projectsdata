exports = function(payload){
  
  /*Two tasks are being performed
    Updating the lock status
    Inserting the document For Tag_id
  */
   /*For Updating lock status*/     
    const mongodb = context.services.get("mongodb-atlas");
    const itemsCollection = mongodb.db("ASSET_INFO").collection("Lock_Status");
    const doc = EJSON.parse(payload.body.text());
    const new_Tag_id = doc.Tag_id;
    
    const query = { "Tag_id": new_Tag_id};
    const update = {
      "$set": {
        "Lock_Status": "1"
      }
    };
    const options = { "upsert": false };// upsert  means update or (insert if field unavaible)
    
    itemsCollection.updateOne(query, update, options)



  /*For Inserting document for Tag_id*/
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("ASSET_INFO").collection("New_Luggage_data");

    
    try {
      if (payload.body) 
      {
        //const ts = {ts:new Date()};
        //const ts_ej = EJSON.stringify(ts);
        body = EJSON.parse(payload.body.text());
        body['ts'] = new Date();
       // body['Lock_Status'] = "1";//Not needed as seperate collection is made for lock status which was necessary to be available even if tag entry is deleted
      }
      coll.insertOne(body);
      console.log(body);

    } catch (e) {
      console.log("Error inserting doc: " + e);
      return e.message();
    }
};