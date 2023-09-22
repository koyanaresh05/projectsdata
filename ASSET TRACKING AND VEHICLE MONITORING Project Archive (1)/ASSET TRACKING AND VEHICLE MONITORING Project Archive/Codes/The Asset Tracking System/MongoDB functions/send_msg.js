exports = function() {
    const twilio = context.services.get("myTwilio");
    twilio.send({
        to: "+916261613772",
        from: context.values.get("twilioNumber"),
        body: "Hello from Realm!"
    });
};

/*
exports = function(trip, user){
  const twilio = context.services.get("twilio");

  twilio.send({
    to: +916261613772,
    from: context.values.get("twilioNumber"),
    body: "Hi " 
      + "."
  });
};
*/
/*
exports = function(payload){
  const twilio = context.services.get("twilio");
  const OTP =   Math.floor(Math.random() * (8999)) + 1000 ;

  twilio.send({
    to: parseInt(payload.body.text()),
    from: context.values.get("twilioNumber"),
    body: "Hi, Your OTP for Password change request in Conductor device is " 
      + OTP + ". This is a automatically generated message, please do not reply."
  });
};
*/