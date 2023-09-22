exports = function(payload) {
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("BUS_INFO").collection("BUS_newexp_DATA");
  
//const query = { "i": parseInt(payload)};
const query = { "i": parseInt(payload.body.text())};
    const update = {
      "$set": {
        "ts": new Date()
      }
    };
    const options = { "upsert": true };// upsert  means update or (insert if field unavaible)
    
    itemsCollection.updateOne(query, update, options);




}

