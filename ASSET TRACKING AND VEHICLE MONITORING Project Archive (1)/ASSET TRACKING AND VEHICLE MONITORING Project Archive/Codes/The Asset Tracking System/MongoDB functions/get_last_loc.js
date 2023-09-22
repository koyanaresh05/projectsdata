exports = function(payload) {
  
 var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("BUS_INFO").collection("Bus_Data");
    const itemsCollection = atlas.db("ASSET_INFO").collection("New_Luggage_data");
  const doc = itemsCollection.findOne({"module":"GPS"}); 
  
  return doc.last_loc;
  
};
