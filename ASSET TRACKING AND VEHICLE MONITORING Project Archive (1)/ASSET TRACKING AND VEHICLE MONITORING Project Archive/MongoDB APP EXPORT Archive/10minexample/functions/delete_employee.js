exports = function(payload){
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("Employee data");
const query = { "Employee_id": payload.body.text()};

itemsCollection.deleteOne(query);
};