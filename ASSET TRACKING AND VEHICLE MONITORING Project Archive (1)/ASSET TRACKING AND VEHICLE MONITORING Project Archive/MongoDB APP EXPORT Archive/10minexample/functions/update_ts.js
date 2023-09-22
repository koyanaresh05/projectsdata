exports = function(payload) {
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("BUS_INFO").collection("BUS_OBD_DATA");
  
 const count =  itemsCollection.count({"ts":null});
//or
//const count =  itemsCollection.count({"ts":{$exists:false}});


//for( i = 0; i<count ; i++)  //But running for loop takes long time and also all ts are very close...so better run from ESP32
//{
const query = { "ts": null};
    const update = {
      "$set": {
        "ts": new Date()
      }
    };
    const options = { "upsert": true };// upsert  means update or (insert if field unavaible)
    
    itemsCollection.updateOne(query, update, options)
//}

return count;
 
/*

const query = { "Employee_id": emp_id};
const update = {
  "$set": {
    "Contact": new_Contact //I can use seperate doc with "type": "forgot_pass" and then use its "password" field which would have been updated from different function
  }
};
const options = { "upsert": false };// upsert  means update or (insert if field unavaible)

itemsCollection.updateOne(query, update, options)

var myCursor = db.users.findAndModify( { type: 2 } );

const doc = itemsCollection.find({});
while (myCursor.hasNext()) {
   print(tojson(myCursor.next()));
}
doc['ts'] = new Date();*/
}


/*
// This function is the endpoint's request handler.
exports = function({ query, headers, body}, response) {
    // Data can be extracted from the request as follows:

    // Query params, e.g. '?arg1=hello&arg2=world' => {arg1: "hello", arg2: "world"}
    const {arg1, arg2} = query;

    // Headers, e.g. {"Content-Type": ["application/json"]}
    const contentTypes = headers["Content-Type"];

    // Raw request body (if the client sent one).
    // This is a binary object that can be accessed as a string using .text()
    const reqBody = body;

    console.log("arg1, arg2: ", arg1, arg2);
    console.log("Content-Type:", JSON.stringify(contentTypes));
    console.log("Request body:", reqBody);

    // You can use 'context' to interact with other Realm features.
    // Accessing a value:
    // var x = context.values.get("value_name");

    // Querying a mongodb service:
    // const doc = context.services.get("mongodb-atlas").db("dbname").collection("coll_name").findOne();

    // Calling a function:
    // const result = context.functions.execute("function_name", arg1, arg2);

    // The return value of the function is sent as the response back to the client
    // when the "Respond with Result" setting is set.
    return  "Hello World!";
};


*/