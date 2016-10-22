module.exports = [
  {
    "type": "heading",
    "defaultValue": "Tojo Clan Watchface"
  },
  {
    "type": "text",
    "defaultValue": "Configure the watchface here."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colours"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColour",
        "defaultValue": "0x000000",
        "label": "Background Colour"
      },
      {
        "type": "color",
        "messageKey": "TimeColour",
        "defaultValue": "0xFFFFFF",
        "label": "Time Colour"
      },
      {
        "type": "select",
        "messageKey": "CrestColour",
        "label": "Crest Colour",
        "options": [
          {
            "label": "White",
            "value": "0xFFFFFF"
          },
          {
            "label": "Black",
            "value": "0x000000"
          }
        ],
        "defaultValue": "0xFFFFFF"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
        "type": "toggle",
        "messageKey": "ItalicizedTime",
        "label": "Italicized Time",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
